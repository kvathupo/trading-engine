#include <charconv>
#include <cstring>
#include <iostream>
#include <print>
#include <filesystem>
#include <format>
#include <fstream>
#include <ranges>
#include <regex>
#include <system_error>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "KrakenParser.hpp"

// @TODO(kvathupo): Handle std::quoted properly with std::formatter
//  - https://stackoverflow.com/questions/66548657/quote-a-string-using-fmt
//  - https://stackoverflow.com/questions/73984876/stdquoted-without-using-stream
namespace te {

// @TODO(kvathupo): use std::from_chars in granularity calc
// @TODO(kvathupo): add magic_enum as a dep
bool KrakenParser::init(const InitializationConfig& cfg) noexcept {
    bool initialization_succeeded{true};
    switch (cfg.type) {
        case te::InitializationType::FileIo: {
            absolute_file_path = std::filesystem::canonical(std::get<te::FilePath>(cfg.data));
            
            // Grab minimum tick granularity from file name
            std::regex re(R"(_(\d+)\.csv$)");
            std::smatch m;
            if (!std::regex_search(absolute_file_path, m, re)) {
                std::println(std::cerr, 
                    "init failure: Failed to retrieve minimum tick granularity from {}", absolute_file_path);
                initialization_succeeded = false;
            }
            const auto min_tick_min = std::stoul(m[1]);
            min_tick_s = min_tick_min  * 60;
            break;
        }
        default:
            std::println(std::cerr, "Initialization type not supported!");
            initialization_succeeded = false;
            break;
    }
    return initialization_succeeded;
}

bool KrakenParser::validate_str_to_num(std::errc& error_code, const std::string_view& validated_str) {
    if (error_code == std::errc::invalid_argument) {
        std::println(std::cerr, "Failed to parse {}", validated_str);
        return false;
    } else if (error_code == std::errc::result_out_of_range) {
        std::println(std::cerr, "Parsed value {} out of numeric range", validated_str);
        return false;
    }
    return true;
}

/*
 *  Requires:
 *      - High price exceed low price
 *      - Duration between timestamps is an integer multiple of the 
 *  minimum granularity. 
 */
bool KrakenParser::is_data_good() {
    // Assume file I/O for now
    if (absolute_file_path.empty()) {
        std::println("absolute_file_path variable was unexpectedly empty!");
        return false;
    }
    std::ifstream fstrm(absolute_file_path);

    // Validate file as we read each row
    std::string out_s;
    std::optional<std::uint32_t> prev_sec_since_epoch{std::nullopt};
    std::size_t row_num{0};
    while (std::getline(fstrm, out_s)) {
        ++row_num;
        // Columns in a row take the following form:
        //  <unix time, open, high, low, close, volume, trades>
        // Note that fractional shares imply volume <= trades
        std::vector<std::string> column_members = std::views::split(out_s, ',') |
            std::ranges::to<std::vector<std::string>>();
        
        // Parse and check high price is greater than low price
        constexpr std::size_t idx_of_high_price{2};
        double high_price{std::numeric_limits<double>::min()}, low_price{std::numeric_limits<double>::min()};
        auto parse_result = std::from_chars(column_members[idx_of_high_price].data(), 
            column_members[idx_of_high_price].data() + column_members[idx_of_high_price].size(),
            high_price, std::chars_format::general);
        if (!validate_str_to_num(parse_result.ec, column_members[idx_of_high_price]))
            return false;

        constexpr std::size_t idx_of_low_price{3};
        parse_result = std::from_chars(column_members[idx_of_low_price].data(), 
            column_members[idx_of_low_price].data() + column_members[idx_of_low_price].size(),
            low_price, std::chars_format::general);
        if (!validate_str_to_num(parse_result.ec, column_members[idx_of_low_price]))
            return false;

        if (low_price > high_price) {
            std::println(std::cerr, "Low price {} exceeds high price {}", low_price, high_price);
            return false;
        }

        // Parse and check duration between ticks respects the minimum granularity
        constexpr std::size_t idx_of_time{0};
        std::uint32_t seconds_since_epoch{0};
        parse_result = std::from_chars(column_members[idx_of_time].data(), 
            column_members[idx_of_time].data() + column_members[idx_of_time].size(),
            seconds_since_epoch);
        if (!validate_str_to_num(parse_result.ec, column_members[idx_of_time]))
            return false;

        if (!prev_sec_since_epoch.has_value()) {
            prev_sec_since_epoch = seconds_since_epoch;
            continue;
        }
        if ((seconds_since_epoch - *prev_sec_since_epoch) % min_tick_s != 0) {
            std::println(std::cerr, "Parse failure at row {}: tick duration {} not a multiple of granularity {}", 
                row_num, seconds_since_epoch - *prev_sec_since_epoch, min_tick_s);
            return false;
        }
    }
    return true;
}

Exchange KrakenParser::get_exchange() {
    return Exchange::Kraken;
}

std::string KrakenParser::get_ticker() {
    // Assume file I/O for now
    if (absolute_file_path.empty()) {
        std::println(std::cerr, "Cannot get ticker from empty file name");
        return "";
    }

    // From a path, capture just the file name (exlude the forward slash)
    std::regex re(R"(([^/]+)$)");
    std::smatch m;
    if (!std::regex_search(absolute_file_path, m, re)) {
        std::println(std::cerr, 
            "get_ticker failure: failed to match from end string to forward slash for file {}",
            absolute_file_path);
        return "";
    }
    
    // Capture all non-underscore characters from the beginning of the file name
    re = "(^([^_]+))";
    std::string regex_result(m.str());
    if (!std::regex_search(regex_result, m, re))
        return "";
    return m.str();
}

KrakenParser::~KrakenParser() {
    if (mmap_address != nullptr && num_bytes_in_file > 0) {
        ::munmap(const_cast<char*>(mmap_address), num_bytes_in_file);
    }
}

bool KrakenParser::tick() {
    /*
     *  1. If prices are empty, attempt to read in 32 prices
     *  2. Else if index not equal to 31, increment index. If new price
     *  is not -1, return true.
     *  3. Else, attempt to read in 32 prices
     */
    const std::size_t price_buffer_size{prices.size()};
    constexpr float kSentinel{-1.0f};

    // Populate mmap pointer on first call
    if (mmap_address == nullptr) {
        if (absolute_file_path.empty()) {
            std::println(std::cerr, "tick: absolute_file_path is empty");
            return false;
        }
        const int file_descriptor = ::open(absolute_file_path.c_str(), O_RDONLY);
        if (file_descriptor < 0) {
            std::println(std::cerr, "tick: open failed for {}", absolute_file_path);
            return false;
        }
        struct stat file_metadata{};
        if (::fstat(file_descriptor, &file_metadata) < 0) {
            ::close(file_descriptor);
            std::println(std::cerr, "tick: failed to populate file metadata for {}", absolute_file_path);
            return false;
        }
        num_bytes_in_file = file_metadata.st_size;
        if (num_bytes_in_file == 0) {
            ::close(file_descriptor);
            std::println(std::cerr, "tick: file {} is empty", absolute_file_path);
            return false;
        }
        // Don't use MAP_POPULATE with MADV_SEQUENTIAL. The latter puts all mmap'd page table entries into the kernel's
        // page reclamation algorithm's inactive LRU. With many open CSVs encroaching on RAM size, this can cause the 
        // reclamation of pages that currently being pointed to by parsers. Thus, a major page fault anyways after a page
        // walk!
        __off_t offset {0};
        void* maybe_address = ::mmap(nullptr, num_bytes_in_file, PROT_READ, MAP_SHARED_VALIDATE, file_descriptor, offset);
        ::close(file_descriptor);
        if (maybe_address == MAP_FAILED) {
            num_bytes_in_file = 0;
            std::println(std::cerr, "tick: mmap failed for {}", absolute_file_path);
            return false;
        }
        mmap_address = static_cast<const char*>(maybe_address);
        ::madvise(const_cast<char*>(mmap_address), num_bytes_in_file, MADV_SEQUENTIAL);
        // Fall through to load the first chunk.
    }  else if (prices_idx + 1 < price_buffer_size) {
        ++prices_idx;
        return prices[prices_idx] != kSentinel;
    }


    // Read 32 new price values 
    // Columns: <unix time, open, high, low, close, volume, trades>
    constexpr std::size_t idx_of_time{0};
    constexpr std::size_t idx_of_close{4};
    std::size_t num_rows_parsed{0};
    while (num_rows_parsed < price_buffer_size && mmap_cursor < num_bytes_in_file) {
        // Create a string view for the whole row
        const char* line_start = mmap_address + mmap_cursor;
        const char* pointer_to_newline = static_cast<const char*>(
            std::memchr(line_start, '\n', num_bytes_in_file - mmap_cursor));
        const std::size_t line_len = pointer_to_newline 
            ? static_cast<std::size_t>(pointer_to_newline - line_start)
            : (num_bytes_in_file - mmap_cursor);        // Hit EOF
        std::string_view line(line_start, line_len);
        // windows-written text files prepend a carriage return to a newline
        if (line.ends_with('\r')) line.remove_suffix(1);
        if (line.empty()) break;     // If we hit EOF

        // Move cursor to next row or EOF
        mmap_cursor += line_len + (pointer_to_newline ? 1 : 0);
        ++csv_row_idx;

        std::array<std::string_view, 7> column_entries{};
        std::size_t col_idx{0};
        for (auto&& col_entry : line | std::views::split(',')) {
            if (col_idx >= column_entries.size()) {
                std::println(std::cerr, "tick: Unexpectly parsed less than 7 column entries in row {}", csv_row_idx);
                return false;
            }
            column_entries[col_idx++] = std::string_view{col_entry};
        }
        std::uint32_t epoch_s{0};
        auto t_res = std::from_chars(column_entries[idx_of_time].data(),
            column_entries[idx_of_time].data() + column_entries[idx_of_time].size(), epoch_s);
        if (!validate_str_to_num(t_res.ec, column_entries[idx_of_time])) {
            std::println(std::cerr, "tick: Fatal error parsing time from row {}", csv_row_idx);
            return false;
        } 

        float price{0.0f};
        auto c_res = std::from_chars(column_entries[idx_of_close].data(),
            column_entries[idx_of_close].data() + column_entries[idx_of_close].size(),
            price, std::chars_format::general);
        if (!validate_str_to_num(c_res.ec, column_entries[idx_of_close])) {
            std::println(std::cerr, "tick: Datal error parsing closing price from row {}", csv_row_idx);
            return false;
        }

        prices[num_rows_parsed] = price;
        price_times[num_rows_parsed] = std::chrono::sys_seconds{
            std::chrono::seconds{epoch_s}};
        ++num_rows_parsed;
    }
    // Reached EOF
    if (num_rows_parsed == 0) return false;

    // Set sentinel values
    for (std::size_t i = num_rows_parsed; i < price_buffer_size; ++i) {
        prices[i] = kSentinel;
    }
    prices_idx = 0;
    return true;
}

std::optional<float> KrakenParser::get_newest_price() {
    if (absolute_file_path.empty())
        return {};
    return prices[prices_idx];
}

std::chrono::sys_seconds KrakenParser::get_newest_time()  {
    if (absolute_file_path.empty())
        return min_sys_time;
    return price_times[prices_idx];
}

// @TODO(kvathupo): return the tick duration relative to the last step?
// Might need to redefine the interface here
std::optional<std::size_t> KrakenParser::get_tick_duration() {
    return {}; 
}

std::optional<float> KrakenParser::get_transaction_fee() {
    return {};
}

std::optional<std::vector<float>> KrakenParser::get_order_book(OrderBookSide side,
    std::uint_fast8_t depth) {
    return {};
}


}
#include <charconv>
#include <iostream>
#include <print>
#include <filesystem>
#include <format>
#include <fstream>
#include <ranges>
#include <regex>
#include <system_error>
#include "KrakenParser.hpp"

// @TODO(kvathupo): Handle std::quoted properly with std::formatter
//  - https://stackoverflow.com/questions/66548657/quote-a-string-using-fmt
//  - https://stackoverflow.com/questions/73984876/stdquoted-without-using-stream
namespace te {

// @TODO(kvathupo): use std::from_chars in granularity calc
// @TODO(kvathupo): add magic_enum as a dep
bool KrakenParser::init(const InitializationConfig& cfg) {
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

std::optional<float> KrakenParser::get_newest_price() {
    if (absolute_file_path.empty())
        return {};
    return prices[buffer_idx];
}

std::optional<std::size_t> KrakenParser::get_newest_time()  {
    if (absolute_file_path.empty())
        return {};
    return epoch_times[buffer_idx];
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
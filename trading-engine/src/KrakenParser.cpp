#include <iostream>
#include <print>
#include <filesystem>
#include <format>
#include <fstream>
#include <ranges>
#include <regex>
#include "KrakenParser.hpp"

namespace te {

// @TODO: add magic_enum as a dep
bool KrakenParser::init(const InitializationConfig& cfg) {
    switch (cfg.type) {
        case te::InitializationType::FileIo:
            file_path = std::filesystem::canonical(std::get<te::FilePath>(cfg.data));
            break;
        default:
            std::println(std::cerr, "Initialization type not supported!");
            return false;
    }

    return true;
}

bool KrakenParser::is_data_good() {
    /*
     *  @Requires:
     *      - Fixed tick duration
     *      - High >= Low
     */
    if (file_path.empty())
        return false;
    
    std::ifstream fstrm(file_path);
    std::string out_s;
    while (std::getline(fstrm, out_s)) {
        std::vector<std::string> column_members = std::views::split(out_s, ',') | std::ranges::to<std::vector<std::string>>();
        std::println("column_members.size() == {}", column_members.size());
        std::println("\tcolumn_members[0] == unix_time == {}", column_members[0]);
        std::println("\tcolumn_members[2] == high == {}", column_members[2]);
        std::println("\tcolumn_members[3] == low == {}", column_members[3]);
    }
    return true;
}

Exchange KrakenParser::get_exchange() {
    return Exchange::Kraken;
}

std::string KrakenParser::get_ticker() {
    // capture from the end of string to forward slash (exclusive)
    std::regex re(R"(([^/]+)$)");
    std::smatch m;

    // Fails on empty string
    if (!std::regex_search(file_path, m, re))
        return "";
    
    // capture all chars not underscore from string beginning
    re = "(^([^_]+))";
    std::string regex_result(m.str());
    if (!std::regex_search(regex_result, m, re))
        return "";
    return m.str();
}

std::optional<float> KrakenParser::get_newest_price() {
    if (file_path.empty())
        return {};
    return prices[buffer_idx];
}

std::optional<std::size_t> KrakenParser::get_newest_time()  {
    if (file_path.empty())
        return {};
    return epoch_times[buffer_idx];
}

// historical data has fixed tick duration
std::optional<std::size_t> KrakenParser::get_tick_duration() {
    return epoch_times[1] - epoch_times[0];
}

std::optional<float> KrakenParser::get_transaction_fee() {
    return {};
}

std::optional<std::vector<float>> KrakenParser::get_order_book(OrderBookSide side,
    std::uint_fast8_t depth) {
    return {};
}


}
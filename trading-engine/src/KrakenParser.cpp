#include <iostream>
#include <print>
#include <format>
#include <regex>
#include "KrakenParser.hpp"

namespace te {

// @TODO: add magic_enum as a dep
bool KrakenParser::init(const InitializationConfig& cfg) {
    switch (cfg.type) {
        case te::InitializationType::FileIo:
            file_name = std::get<te::FilePath>(cfg.data);
            std::println("File type supported!");
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
    return true;
}

Exchange KrakenParser::get_exchange() {
    return Exchange::Kraken;
}

std::string KrakenParser::get_ticker() {
    std::regex re(R"(([^/]+)$)");
    std::smatch m;

    // Fails on empty string
    if (!std::regex_search(file_name, m, re))
        return "";
    re = "(^([^_]+))";
    std::string regex_result(m.str());
    if (!std::regex_search(regex_result, m, re))
        return "";
    return m.str();
}

std::optional<float> KrakenParser::get_newest_price() {
    if (file_name.empty())
        return {};
    return prices[buffer_idx];
}

std::optional<std::size_t> KrakenParser::get_newest_time()  {
    if (file_name.empty())
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
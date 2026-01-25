#include "KrakenParser.hpp"

namespace te {

bool KrakenParser::init(InitializationConfig cfg) {
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
    return "";
}

std::optional<float> KrakenParser::get_newest_price() {
    return {};
}

std::optional<std::size_t> KrakenParser::get_newest_time()  {
    return {};
}

// historical data has fixed tick duration
std::optional<std::size_t> KrakenParser::get_tick_duration() {
    return newest_time[1] - newest_time[0];
}

std::optional<float> KrakenParser::get_transaction_fee() {
    return {};
}

std::optional<std::vector<float>> KrakenParser::get_order_book(OrderBookSide side,
    std::uint_fast8_t depth) {
    return {};
}



}
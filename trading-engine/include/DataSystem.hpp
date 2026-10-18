#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>

#include "DataParser.hpp"
#include "Types.hpp"

namespace te {

/*
 *  Responsible for maintaining connections to data (live exchange or historical data),
 *  and exposing pricing information to the `PriceSystem`.
 *
 *  No methods should throw exceptions. All are synchronous.
 */
class DataSystem {
public:
    /*
     *  @param execution_mode
     *      If using real-time data, exchange connections are opened. Else, historical
     *  data is read from disk.
     */
    DataSystem(const ExecutionMode execution_mode);

    /*
     *  @param start_date
     */
    bool init(const std::chrono::year_month_day start_date);

    /*
     *  If historical data, ticks all data parsers to a time less than or equal to `curr_time`.
     *  Else, grabs the newest data from the exchange.
     *
     *  @param curr_time The time to tick to.
     */
    bool tick(const TimePoint& curr_time);

    /** Adds a `parser` for data from `exchange`. */
    bool add_data_parser(Exchange exchange, std::unique_ptr<DataParser> parser);

    /*
     *  Queries exchange for most recent price data. If using historical data,
     *  the price is the most recent price less than or equal to `newest_time`.
     *
     *  @param exchange Exchange for ticker
     *  @param ticker Ticker on exchange
     *  @param request_time Required with historical data to prevent peeking
     *  at future prices.
     *
     *  @return
     *      - If historical data, populated optional if there's price data at
     *  a time less than or equal to the requested time.
     *      - If live data, populated optional on connectivity.
     */
    std::optional<float> get_most_recent_price_for(Exchange exchange, std::string ticker,
                                                   std::optional<TimePoint> request_time);

    ExecutionMode mExecution_mode;
    std::vector<Exchange> mExchanges{};

    // Kraken exchange
    std::unordered_map<std::string, std::unique_ptr<DataParser>> mTickerToKrakenParser;
};

} // end namespace te

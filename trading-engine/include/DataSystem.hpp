#pragma once

#include "Types.hpp"
#include "DataParser.hpp"

#include <chrono>
#include <memory>
#include <vector>
#include <unordered_map>


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
    bool tick(const std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>& curr_time);

    /** Adds a `parser` for data from `exchange`. */
    bool add_data_parser(Exchange exchange, std::unique_ptr<DataParser> parser);

    ExecutionMode mExecution_mode;
    std::vector<Exchange> mExchanges {};
    std::unordered_map<Exchange, std::vector<std::unique_ptr<DataParser>>> mExchangeToDataSources {};
};

}   // end namespace te

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
    DataSystem() = default;
    
    /*
     *  @param execution_mode
     *      If using real-time data, exchange connections are opened. Else, historical
     *  data is read from disk.
     *  @param exchanges
     *      List of exchanges to connect to.
     *  @param start_date
     *      
     */
    bool init(const ExecutionMode execution_mode, const std::vector<Exchange>& exchanges,
        const std::chrono::year_month_day start_date);

    bool add_data_parser(Exchange exchange, std::unique_ptr<DataParser>);

    ExecutionMode mExecution_mode;
    std::vector<Exchange> mExchanges;
    std::unordered_map<Exchange, std::vector<std::unique_ptr<DataParser>>> mExchangeToDataSources;
};

}   // end namespace te

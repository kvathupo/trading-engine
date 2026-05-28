#pragma once

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
     *  @param exchanges
     *      List of exchanges to connect to.
     *  @param start_date
     *      
     */
    DataSystem(const ExecutionMode execution_mode, const std::vector<Exchange>& exchanges,
        const std::chrono::year_month_day start_date);

    ExecutionMode execution_mode;
};

}   // end namespace te

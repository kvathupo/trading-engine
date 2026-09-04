#pragma once
#include <chrono>
#include <memory>
#include <utility>

#include "Types.hpp"
#include "DataSystem.hpp"


namespace te {

struct World {
    /*
     *  @param execution_mode
     */
    World(const ExecutionMode execution_mode);

    /** 
     *  Returns false if already initialized, invalid start date, or any
     *  systems fail to initialize.
     */
    bool init(const std::chrono::year_month_day& start,
        const std::size_t& num_days);
    bool tick(const std::chrono::seconds& delta_time);

    bool add_data(Exchange exchangeForParser, std::unique_ptr<DataParser> parser);

    /** Systems */
    DataSystem dataSystem;

    std::optional<std::chrono::year_month_day> start = {};
    std::optional<std::chrono::minutes> duration = {};
    // @TODO(kvathupo): Move to dedicated header to not repeat time defs in datasystem, etc?
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> curr_time;
};

}       // end namespace te

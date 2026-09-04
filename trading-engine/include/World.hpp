#pragma once
#include <chrono>
#include <utility>
#include <vector>

#include "Types.hpp"
#include "DataSystem.hpp"


namespace te {

struct World {
    World() = default;
    /**
     *  Returns false if already initialized, invalid start date, or the data
     *  system failed to initialize.
     *
     *  @param execution_mode
     *      Forwarded to the data system: selects historical vs. real-time data.
     *  @param exchanges
     *      Forwarded to the data system: exchanges to source data from.
     */
    bool init(const std::chrono::year_month_day& start,
        const std::size_t& num_days,
        const ExecutionMode execution_mode,
        const std::vector<Exchange>& exchanges);
    bool tick(const std::chrono::seconds& delta_time);

/*  
 *  @TODO(kvathupo): Initialize DataParser pointing to that path
 *  bool add_data(const std::string& path_to_dir, std::unique_ptr<DataParser> parser)
 *
 */

    /** Systems */
    DataSystem dataSystem;

    std::optional<std::chrono::year_month_day> start = {};
    std::optional<std::chrono::minutes> duration = {};
    // @TODO(kvathupo): Move to dedicated header to not repeat time defs in datasystem, etc?
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> curr_time;
};

}       // end namespace te

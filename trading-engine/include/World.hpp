#pragma once
#include <chrono>
#include <utility>

#include "Types.h"


namespace te {

struct World {
    World() = default;
    /** Returns false if already initialized or invalid start date */
    bool init(const std::chrono::year_month_day& start,
        const std::size_t& num_days);
    bool tick(const std::chrono::seconds& delta_time);

/*  
 *  @TODO(kvathupo): Initialize DataParser pointing to that path
 *  bool add_data(const std::string& path_to_dir, std::shared_ptr<DataParser> parser)
 *
 */
    std::optional<std::chrono::year_month_day> start = {};
    std::optional<std::chrono::minutes> duration = {};
    // @TODO(kvathupo): Move to dedicated header to not repeat time defs in datasystem, etc?
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> curr_time;
};

}       // end namespace te

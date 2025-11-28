#include "World.hpp"
#include <print>

namespace te {

// @TODO(kvathupo): use std::println() with cerr
bool World::init(const std::chrono::year_month_day& start,
        const std::size_t& num_days) {
    if (this->start || duration) {
        std::println("Already initialized: ignoring");
        return false;
    }
    if (!start.ok()) {
        std::println("{} not a valid day: ignoring", start);
        return false;
    }
    this->start = start;
    duration = std::chrono::days(1) * num_days;
    curr_time = std::chrono::sys_days(start);

    // @TODO(kvathupo): In DataSystem::init(),
    // i. Early-out if DataSystem has no DataParser
    // ii. iterate over all DataParsers. Populate curr_time
    // with whatever time we find.

    return true;
}

bool World::tick(const std::chrono::seconds& delta_time) {
    // @TODO(kvathupo): tick systems
    curr_time += delta_time;
    return true;
}

}

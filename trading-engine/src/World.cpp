#include "World.hpp"
#include <print>

namespace te {

bool World::init(const std::chrono::year_month_day& start,
        const std::size_t& num_days) {
    if (this->start || duration) {
        std::println("Already initialized: ignoring");
        return false;
    }
    this->start = start;
    duration = std::chrono::days(1) * num_days;

    // @TODO(kvathupo): In DataSystem::init(),
    // i. Early-out if DataSystem has no DataParser
    // ii. iterate over all DataParsers. Populate curr_time
    // with whatever time we find.

    return true;
}

bool World::tick(const std::chrono::duration<double>& deltaTime) {
    return true;
}

}

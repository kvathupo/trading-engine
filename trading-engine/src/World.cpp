#include "World.hpp"

namespace te {

bool World::init(const std::chrono::year_month_day& start,
        const std::chrono::years& duration) {
    return true;
}

bool World::tick(const std::chrono::duration<double>& deltaTime) {
    return true;
}

}

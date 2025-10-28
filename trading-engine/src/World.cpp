#include "World.hpp"

namespace te {

bool World::init(const std::chrono::year_month_day& start,
        const std::chrono::years& duration) {
    return true;
}

template<typename T>
bool World::tick(const std::chrono::duration<T>& deltaTime) {
    return true;
}

}

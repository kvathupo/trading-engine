#include "World.hpp"

#include <iostream>
#include <print>

namespace te {

World::World(const ExecutionMode execution_mode):
    dataSystem{execution_mode}
{}

// @TODO(kvathupo): use std::println() with cerr
bool World::init(const std::chrono::year_month_day& start,
        const std::size_t& num_days) {
    if (this->start || duration) {
        std::println("Already initialized: ignoring");
        return false;
    }
    if (!start.ok()) {
        std::println(std::cerr, "{} not a valid day", start);
        return false;
    }

    // Init systems before taking on any state, so a failure leaves the world
    // uninitialized and retryable.
    if (!dataSystem.init(start)) {
        std::println(std::cerr, "Data system failed to initialize");
        return false;
    }
    this->start = start;
    duration = std::chrono::days(1) * num_days;
    curr_time = std::chrono::sys_days(start);

    return true;
}

bool World::add_data(Exchange exchangeForParser, std::unique_ptr<DataParser> parser) {
    return dataSystem.add_data_parser(exchangeForParser, std::move(parser));
}

bool World::tick(const std::chrono::seconds& delta_time) {
    // @TODO(kvathupo): tick systems
    curr_time += delta_time;

    if (!dataSystem.tick(curr_time)) {
        return false;
    }
    return true;
}

}

#pragma once
#include <chrono>
#include <memory>
#include <utility>

#include "DataSystem.hpp"
#include "PricingSystem.hpp"
#include "Types.hpp"

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
    bool init(const std::chrono::year_month_day& start, const std::size_t& num_days);
    bool tick(const std::chrono::seconds& delta_time);

    bool add_data_parser(Exchange exchangeForParser, std::unique_ptr<DataParser> parser);
    TimePoint get_curr_time() const;

    /** Systems */
    DataSystem dataSystem;
    PricingSystem pricingSystem;

    std::optional<std::chrono::year_month_day> start = {};
    std::optional<std::chrono::minutes> duration = {};
    const ExecutionMode mExecution_mode;

private:
    TimePoint curr_time;
};

} // end namespace te

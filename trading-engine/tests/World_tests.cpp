#include <gtest/gtest.h>
#include "World.hpp"

using namespace std::chrono;
using namespace te;

TEST(WorldTest, init) {
    World w;
    const year_month_day start_date = year{2025}/January/1d;
    const std::size_t num_days {2};
    EXPECT_TRUE(w.init(start_date, num_days));
    EXPECT_FALSE(w.init(start_date, num_days + 2));
}

TEST(WorldTest, tick) {
    World w;
    const year_month_day start_date = year{2025}/January/1d;
    const std::size_t num_days {2};
    EXPECT_TRUE(w.init(start_date, num_days));

    const seconds expected_delta_time_s{60};
    const std::size_t minutes_in_day(1440);
    for (std::size_t min = 0; min < minutes_in_day; ++min) {
        const auto prev_time(w.curr_time);
        EXPECT_TRUE(w.tick(expected_delta_time_s));
        const auto curr_time(w.curr_time);
        const auto delta_time_s = duration_cast<seconds>(curr_time - prev_time);
        EXPECT_EQ(delta_time_s, expected_delta_time_s);
    }
    const year_month_day expected_end_date = year{2025}/January/2d;
    EXPECT_EQ(w.curr_time, sys_days(expected_end_date));
}

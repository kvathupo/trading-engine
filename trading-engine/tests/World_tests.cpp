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

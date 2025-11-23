#include <gtest/gtest.h>
#include "World.hpp"

TEST(WorldTest, tick) {
    using namespace te;
    World w;
    std::chrono::seconds s(1);
    EXPECT_TRUE(w.tick(s));
}

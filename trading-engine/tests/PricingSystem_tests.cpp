#include <chrono>
#include <cstddef>
#include <format>
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "KrakenParser.hpp"
#include "World.hpp"

using namespace std::chrono;
using namespace te;

/*
 *  Tests that the pricing system doesn't peek at future prices,
 *  and updates prices correctly.
 */
TEST(PricingSystemTest, correct_historical_price) {
    World world{ExecutionMode::Backtest};

    // Add parser for Kraken BTT/USD swap stepping at 5 minutes
    auto parser = std::make_unique<KrakenParser>();
    ASSERT_TRUE(parser->init({.data = std::string(TEST_DATA_DIR) + "kraken/OHLCVT_Q1_2023/BTTUSD_5.csv",
                              .type = InitializationType::FileIo}));
    const auto tick_duration = parser->get_tick_duration();
    ASSERT_TRUE(tick_duration.has_value());
    const std::size_t parser_tick_duration_s = tick_duration->count();
    const std::string ticker = parser->get_ticker();
    ASSERT_TRUE(world.add_data_parser(Exchange::Kraken, std::move(parser)));

    const TimePoint historicalDataStartTime{seconds{1672532100}};

    // Start the world the day before the data begins
    const year_month_day start_date = year{2022} / December / 31d;
    ASSERT_TRUE(world.init(start_date, 2));
    EXPECT_FALSE(world.pricingSystem.get_most_recent_price_for(Exchange::Kraken, ticker).has_value());
    EXPECT_EQ(world.get_curr_time(), sys_days{start_date});

    // No price until world time reaches the first data point
    const seconds step(parser_tick_duration_s);
    for (TimePoint t = world.get_curr_time(); t < historicalDataStartTime; t = world.get_curr_time()) {
        EXPECT_FALSE(world.pricingSystem.get_most_recent_price_for(Exchange::Kraken, ticker).has_value())
            << std::format("Price exposed at {}, before data starts at {}", t, historicalDataStartTime);
        ASSERT_TRUE(world.tick(step));
    }
    ASSERT_EQ(world.get_curr_time(), historicalDataStartTime);

    // No price update within a single duration
    const auto first_price = world.pricingSystem.get_most_recent_price_for(Exchange::Kraken, ticker);
    ASSERT_TRUE(first_price.has_value());

    const seconds half_step(parser_tick_duration_s / 2);
    ASSERT_TRUE(world.tick(half_step));
    const auto held_price = world.pricingSystem.get_most_recent_price_for(Exchange::Kraken, ticker);
    ASSERT_TRUE(held_price.has_value());
    EXPECT_FLOAT_EQ(*held_price, *first_price);

    // Price remains the same until 12-31-2022 01:50:00 UTC
    const TimePoint priceChangeTime{seconds{1672537800}};
    for (TimePoint t = world.get_curr_time(); t < priceChangeTime; t = world.get_curr_time()) {
        const auto unchanged_price = world.pricingSystem.get_most_recent_price_for(Exchange::Kraken, ticker);
        ASSERT_TRUE(unchanged_price.has_value());
        EXPECT_FLOAT_EQ(*unchanged_price, *first_price) << std::format("Price unexpectedly changed at {}", t);
        ASSERT_TRUE(world.tick(half_step));
    }
    ASSERT_EQ(world.get_curr_time(), priceChangeTime);

    const auto next_price = world.pricingSystem.get_most_recent_price_for(Exchange::Kraken, ticker);
    ASSERT_TRUE(next_price.has_value());
    EXPECT_NE(*next_price, *first_price);
}

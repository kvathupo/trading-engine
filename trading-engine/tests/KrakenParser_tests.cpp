#include <filesystem>
#include <algorithm>

#include "Types.hpp"
#include "KrakenParser.hpp"

#include <gtest/gtest.h>

namespace te {

/*
 *  Tests that historical data can be successfully parsed.
 */
TEST(KrakenParserTests, parse) {
    te::KrakenParser parser;

    constexpr unsigned long expected_min_tick_s{60 * 60};
    const std::string historicalDataPathWithDots = std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/BTTUSD_60.csv";

    te::InitializationConfig initConfig = {
        .data = historicalDataPathWithDots,
        .type = te::InitializationType::FileIo
    };


    EXPECT_TRUE(parser.absolute_file_path.empty()) << "Incorrect value prior to initialization";
    EXPECT_EQ(parser.min_tick_s, 0) << "Incorrect value prior to initialization";
    EXPECT_EQ(parser.buffer_idx, 0) << "Incorrect value prior to initialization";
    EXPECT_TRUE(std::ranges::all_of(parser.prices, [](const auto price) { static int i = 0; ++i; return price == 0; }));
    EXPECT_TRUE(std::ranges::all_of(parser.price_times, [](const auto time) { return time.time_since_epoch().count() == 0 ; }));

    EXPECT_TRUE(parser.init(initConfig));

    // Check member variables
    EXPECT_EQ(parser.absolute_file_path, std::filesystem::canonical(historicalDataPathWithDots));
    EXPECT_EQ(parser.min_tick_s, expected_min_tick_s);
    // TODO
    /*
    // ?
    EXPECT_EQ(parser.buffer_idx, 0);
    EXPECT_TRUE(std::ranges::none_of(parser.prices, [](const auto price) { static int i = 0; ++i; return price == 0; }));
    EXPECT_TRUE(std::ranges::none_of(parser.epoch_times, [](const auto price) { return price == 0; }));
    */

    EXPECT_EQ(parser.get_ticker(), "BTTUSD");
    EXPECT_TRUE(parser.is_data_good());

    SUCCEED();
}

/*
 *  Tests that parsers do not throw exceptions on failures.
 */


}   // end namespace te
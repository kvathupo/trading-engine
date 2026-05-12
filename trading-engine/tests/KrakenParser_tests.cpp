#include <array>
#include <cstdint>
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
    using namespace std::chrono;
    // Prices are in the 1e-7 range
    te::KrakenParser parser;

    constexpr unsigned long expected_min_tick_s{60 * 60};
    const std::string historicalDataPathWithDots = std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/BTTUSD_60.csv";

    te::InitializationConfig initConfig = {
        .data = historicalDataPathWithDots,
        .type = te::InitializationType::FileIo
    };


    EXPECT_TRUE(parser.absolute_file_path.empty()) << "Incorrect value prior to initialization";
    EXPECT_EQ(parser.min_tick_s, 0) << "Incorrect value prior to initialization";
    EXPECT_EQ(parser.prices_idx, 0) << "Incorrect value prior to initialization";
    EXPECT_TRUE(std::ranges::all_of(parser.prices, [](const auto price) { static int i = 0; ++i; return price == 0; }));
    EXPECT_TRUE(std::ranges::all_of(parser.price_times, [](const auto time) { return time.time_since_epoch().count() == 0 ; }));

    EXPECT_TRUE(parser.init(initConfig));

    // Check member variables
    EXPECT_EQ(parser.absolute_file_path, std::filesystem::canonical(historicalDataPathWithDots));
    EXPECT_EQ(parser.min_tick_s, expected_min_tick_s);
    EXPECT_EQ(parser.get_ticker(), "BTTUSD");
    EXPECT_TRUE(parser.is_data_good());

    // The `KrakenParser` has an internal price/time buffer of size 32. So expect 
    // the buffer to be reloaded twice over 65 rows.
    constexpr std::size_t num_rows{65};
    constexpr std::array<float, num_rows> expected_prices{
        6.3e-07f, 6.2e-07f, 6.3e-07f, 6.2e-07f, 6.3e-07f, 6.2e-07f, 6.2e-07f, 6.3e-07f,
        6.3e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f, 6.2e-07f,
        6.3e-07f, 6.3e-07f, 6.3e-07f, 6.2e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f,
        6.3e-07f, 6.3e-07f, 6.2e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f,
        6.3e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f, 6.2e-07f, 6.3e-07f, 6.3e-07f,
        6.3e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f, 6.2e-07f, 6.3e-07f, 6.3e-07f,
        6.3e-07f, 6.3e-07f, 6.2e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f, 6.2e-07f, 6.2e-07f,
        6.2e-07f, 6.2e-07f, 6.1e-07f, 6.2e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f, 6.3e-07f,
        6.3e-07f
    };
    constexpr std::array<std::int64_t, num_rows> expected_epoch_seconds{
        1672531200, 1672534800, 1672542000, 1672552800, 1672556400, 1672560000, 1672567200, 1672570800,
        1672581600, 1672588800, 1672592400, 1672596000, 1672599600, 1672603200, 1672606800, 1672610400,
        1672614000, 1672621200, 1672628400, 1672632000, 1672635600, 1672639200, 1672642800, 1672646400,
        1672650000, 1672653600, 1672657200, 1672660800, 1672664400, 1672675200, 1672678800, 1672682400,
        1672686000, 1672689600, 1672693200, 1672696800, 1672700400, 1672704000, 1672707600, 1672711200,
        1672714800, 1672718400, 1672722000, 1672725600, 1672729200, 1672732800, 1672736400, 1672743600,
        1672747200, 1672750800, 1672754400, 1672758000, 1672761600, 1672765200, 1672768800, 1672772400,
        1672776000, 1672779600, 1672783200, 1672786800, 1672790400, 1672794000, 1672797600, 1672804800,
        1672808400
    };

    const std::size_t kBufferSize = parser.prices.size();
    for (std::size_t i = 0; i < num_rows; ++i) {
        EXPECT_TRUE(parser.tick()) << "tick() returned false at iteration " << i;
        EXPECT_LT(parser.prices_idx, kBufferSize)
            << "prices_idx out of bounds at iteration " << i;
        EXPECT_EQ(parser.prices_idx, i % kBufferSize)
            << "prices_idx did not match expected position at iteration " << i;
        EXPECT_FLOAT_EQ(*parser.get_newest_price(), expected_prices[i])
            << "Unexpected price at iteration " << i;
        EXPECT_EQ(parser.get_newest_time(), sys_seconds(seconds(expected_epoch_seconds[i])))
            << "Unexpected time at iteration " << i;
    }
}

/*
 *  Tests that parsers do not throw exceptions on failures.
 */


}   // end namespace te
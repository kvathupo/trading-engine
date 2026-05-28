#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ranges>

#include "Types.hpp"
#include "KrakenParser.hpp"

#include <gtest/gtest.h>

namespace te {

/*
 *  Tests that historical data can be successfully parsed.
 */
TEST(KrakenParserTests, parse) {
    using namespace std::chrono;

    // Check parser with no data attached
    te::KrakenParser parser;
    EXPECT_TRUE(parser.absolute_file_path.empty()) << "Incorrect value prior to initialization";
    EXPECT_EQ(parser.min_tick_s, 0) << "Incorrect value prior to initialization";
    EXPECT_EQ(parser.prices_idx, 0) << "Incorrect value prior to initialization";
    EXPECT_TRUE(std::ranges::all_of(parser.prices, [](const auto price) { static int i = 0; ++i; return price == 0; }));
    EXPECT_TRUE(std::ranges::all_of(parser.price_times, [](const auto time) { return time.time_since_epoch().count() == 0 ; }));

    // Check parser post-initialization
    // Prices are in the 1e-7 range
    constexpr unsigned long expected_min_tick_s{60 * 60};
    const std::string historicalDataPathWithDots = std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/BTTUSD_60.csv";
    te::InitializationConfig initConfig = {
        .data = historicalDataPathWithDots,
        .type = te::InitializationType::FileIo
    };
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
 *  Tests that Kraken Parsers correctly parse price levels, times, tickers, and min ticks for 
 *  historical data with time granularities of 1, 5, 15, 60, 720, and 1440 seconds
 */
TEST(KrakenParserTests, parse_multiple_granularities) {
    /*  1:03
     *  1. Given all CSVs in the Kraken dir path, iterate over them. Generate a Kraken parser for each.
     *  2. Check that the ticker and min ticks are correct (have an unordered_map from relative file path
     *  to {string ticker, unsigned long min_tick_s})
     *  3. Check price levels and times.
     *      i. Until EOF, get a row from the csv and tick the KrakenParser. 
     *      ii. Grab the expected price and time from the row -> std::ranges::split(',')
     *      iii. Compare the expected price and time to the KrakenParser. 
     */

    std::unordered_map<std::string, std::pair<std::string, unsigned long>> fileNameToTickerAndMinTick {
        {std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/BTTUSD_5.csv",    {"BTTUSD",  60*5}},
        {std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/BTTUSD_60.csv",   {"BTTUSD",  60*60}},
        {std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/BTTUSD_720.csv",  {"BTTUSD",  60*720}},
        {std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/ETHCAD_1.csv",    {"ETHCAD",  60*1}},
        {std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/ETHDAI_720.csv",  {"ETHDAI",  60*720}},
        {std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/ETHJPY_1.csv",    {"ETHJPY",  60*1}},
        {std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/ETHJPY_720.csv",  {"ETHJPY",  60*720}},
        {std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/ETHUSD_1.csv",    {"ETHUSD",  60*1}},
        {std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/ETHUSDC_1.csv",   {"ETHUSDC", 60*1}},
        {std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/ETHUSDT_1.csv",   {"ETHUSDT", 60*1}},
        {std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/ZECUSD_1.csv",    {"ZECUSD",  60*1}},
        {std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/ZECUSD_15.csv",   {"ZECUSD",  60*15}},
        {std::string(TEST_DATA_DIR) + "./kraken/OHLCVT_Q1_2023/ZECUSD_1440.csv", {"ZECUSD",  60*1440}},
    };

    for (const auto [fileName, tickerAndMinTick] : fileNameToTickerAndMinTick) {
        const auto [expectedTickerName, expectedMinTick_s] = tickerAndMinTick;
        te::InitializationConfig initConfig = {
            .data = fileName,
            .type = te::InitializationType::FileIo
        };
        KrakenParser parser;
        parser.init(initConfig);

        // Check metadata is correct
        EXPECT_EQ(parser.get_exchange(), Exchange::Kraken);
        EXPECT_EQ(parser.get_ticker(), expectedTickerName);
        EXPECT_EQ(parser.get_tick_duration(), std::chrono::seconds(expectedMinTick_s));

        // Check price levels and times
        std::fstream fstrm(fileName);
        std::string row;
        constexpr std::size_t idx_of_time{0};
        constexpr std::size_t idx_of_close{4};
        while (std::getline(fstrm, row)) {
            parser.tick();

            std::vector<std::string> columnsInRow = std::ranges::views::split(row, ',')
                | std::ranges::to<std::vector<std::string>>();
            std::string timeStr = columnsInRow[idx_of_time];
            const unsigned long secondsSinceUnixEpoch = std::stol(timeStr);
            std::chrono::sys_seconds expectedSeconds = std::chrono::sys_seconds(std::chrono::seconds(secondsSinceUnixEpoch));
            EXPECT_EQ(expectedSeconds, parser.get_newest_time());

            std::string priceStr = columnsInRow[idx_of_close];
            const float expectedPrice = std::stod(priceStr);
            ASSERT_TRUE(parser.get_newest_price().has_value());
            EXPECT_FLOAT_EQ(expectedPrice, *parser.get_newest_price());
        }
    }
}


}   // end namespace te
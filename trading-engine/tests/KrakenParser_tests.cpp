#include <gtest/gtest.h>
#include "Types.hpp"
#include "KrakenParser.hpp"


TEST(KrakenParser, parse) {
    te::KrakenParser parser;
    te::InitializationConfig initConfig = {
        .data = "./data/kraken/OHLCVT_Q1_2023/BTTUSD_60.csv",
        .type = te::InitializationType::FileIo
    };
    EXPECT_TRUE(parser.init(initConfig));
    EXPECT_EQ(parser.get_ticker(), "BTTUSD");

    SUCCEED();
}

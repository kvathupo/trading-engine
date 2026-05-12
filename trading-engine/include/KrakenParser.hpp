#pragma once
#include "DataParser.hpp"
#include <array>

#include <gtest/gtest_prod.h>

namespace te {

// @TODO(kvathupola): template on buffer size?
class KrakenParser : public DataParser {
public:
    KrakenParser() = default;
    KrakenParser(const KrakenParser&) = delete;
    KrakenParser& operator=(const KrakenParser&) = delete;
    virtual bool init(const InitializationConfig& cfg) noexcept override;
    virtual bool is_data_good() override;
    virtual ~KrakenParser();

    virtual Exchange get_exchange() override;
    virtual std::string get_ticker() override;

    virtual bool tick() override;
    virtual std::optional<float> get_newest_price() override;
    virtual std::chrono::sys_seconds get_newest_time() override;
    virtual std::optional<std::size_t> get_tick_duration() override;
    virtual std::optional<float> get_transaction_fee() override;
    virtual std::optional<std::vector<float>> get_order_book(OrderBookSide side,
        std::uint_fast8_t depth) override;

protected:
    // If the error code is populated, log a warning and return false
    bool validate_str_to_num(std::errc& error_code, 
        const std::string_view& validated_str);

private:
    std::string absolute_file_path{""};
    // The minimum granularity for a tick
    // There is no guarantee that consecutive rows have a timestamp differing
    // by `min_tick_s`. If there are no trades during a min granularity period,
    // the row is omitted.
    unsigned long min_tick_s{0};

    
    std::array<float, 32> prices{};
    // Index to current `prices` value
    uint8_t prices_idx{0};
    // Historical epoch time granularity in single seconds.
    std::array<std::chrono::sys_seconds, 32> price_times{};

    // Zero-indexed row number corresponding to the price at
    // the end of the queue
    std::size_t csv_row_idx{0};

    // mmap-backed streaming state. Lazily initialized on first tick().
    const char* mmap_address{nullptr};
    // 
    std::size_t num_bytes_in_file{0};
    std::size_t mmap_cursor{0};


    /*
     *  Declare gtest friends
     */
    FRIEND_TEST(KrakenParserTests, parse);
};

}       // end namespace te

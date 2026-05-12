#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "Types.hpp"


namespace te {

constexpr auto min_sys_time = std::chrono::sys_seconds::min();

struct DataParser {
    DataParser() = default;
    /*
     *  Initializes data from feed.
     *
     *  @returns true on success.
     */
    virtual bool init(const InitializationConfig& cfg) noexcept = 0;
    virtual bool is_data_good() = 0;
    virtual ~DataParser() = default;

    virtual Exchange get_exchange() = 0;
    // TODO(optim): return ref to const string
    virtual std::string get_ticker() = 0;

    /*
     *  Update the internal state to the next data point. 
     *  @returns
     *      True on sucess. Else, False on failure or no more data.
     */
    virtual bool tick() = 0;
    virtual std::optional<float> get_newest_price() = 0;
    /*
     *  Returns time corresponding to newest price, else min_sys_time on failure.
     *  Current minimum granularity across data feeds is one second.
     */
    virtual std::chrono::sys_seconds get_newest_time() = 0;
    virtual std::optional<std::size_t> get_tick_duration() = 0;
    virtual std::optional<float> get_transaction_fee() = 0;
    virtual std::optional<std::vector<float>> get_order_book(OrderBookSide side,
        std::uint_fast8_t depth) = 0;
};

}   // end namespace te

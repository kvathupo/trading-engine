#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "Types.h"


namespace te {

class DataParser {
    DataParser() = default;
    virtual bool init(InitializationConfig cfg) = 0;
    virtual ~DataParser() = default;

    // TODO(optim): return ref to const string
    virtual std::string get_exchange() = 0;
    virtual std::string get_ticker() = 0;

    virtual std::optional<float> get_newest_price() = 0;
    virtual std::optional<float> get_newest_time() = 0;
    virtual std::optional<float> get_tick_duration() = 0;
    virtual std::optional<float> get_transaction_fee() = 0;
    virtual std::optional<std::vector<float>> get_order_book(OrderBookSide side,
        std::uint_fast8_t depth) = 0;
};



}   // end namespace te

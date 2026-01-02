#pragma once
#include "DataParser.hpp"

namespace te {

class KrakenParser : public DataParser {
public:
    DataParser() = default;
    virtual bool init(InitializationConfig cfg) override;
    virtual ~DataParser() = default;

    virtual std::string get_exchange() override;
    virtual std::string get_ticker() override;

    virtual std::optional<float> get_newest_price() override;
    virtual std::optional<float> get_newest_time() override;
    virtual std::optional<float> get_tick_duration() override;
    virtual std::optional<float> get_transaction_fee() override;
    virtual std::optional<std::vector<float>> get_order_book(OrderBookSide side,
        std::uint_fast8_t depth) override;
};

}       // end namespace te

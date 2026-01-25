#pragma once
#include "DataParser.hpp"

namespace te {

// @TODO(optim): faster to parse bytes instead of text?
class KrakenParser : public DataParser {
public:
    KrakenParser() = default;
    virtual bool init(const InitializationConfig& cfg) override;
    virtual bool is_data_good() override;
    virtual ~KrakenParser() = default;

    virtual Exchange get_exchange() override;
    virtual std::string get_ticker() override;

    virtual std::optional<float> get_newest_price() override;
    virtual std::optional<std::size_t> get_newest_time() override;
    virtual std::optional<std::size_t> get_tick_duration() override;
    virtual std::optional<float> get_transaction_fee() override;
    virtual std::optional<std::vector<float>> get_order_book(OrderBookSide side,
        std::uint_fast8_t depth) override;

private:
    std::string file_name{""};
    
    // Maintain size of at least 2 to grab tick duration
    uint8_t buffer_idx {0};
    float newest_price[32];
    std::size_t newest_time[32];
};

}       // end namespace te

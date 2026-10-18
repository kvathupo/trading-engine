#include "PricingSystem.hpp"

#include "DataSystem.hpp"

namespace te {

namespace {

// Dummy price held by each ticker until the exchange is queried
constexpr float kSentinelPrice{-1.0f};

} // namespace

bool PricingSystem::init() {
    return true;
}

bool PricingSystem::tick(const DataSystem& dataSystem, const TimePoint curr_time) {
    // query the price on demand
    if (mExecution_mode != ExecutionMode::Backtest) {
        return true;
    }

    populateActiveTickers(dataSystem, curr_time);
    return true;
}

void PricingSystem::populateActiveTickers(const DataSystem& dataSystem, const TimePoint curr_time) {
    for (const auto& krakenTicker : mTickersKraken) {
        std::optional<float> maybePrice =
            dataSystem.get_most_recent_price_for(Exchange::Kraken, krakenTicker, curr_time);
        if (maybePrice) {
            mTickerToPriceKraken[krakenTicker] = *maybePrice;
        }
    }
    return;
}

bool PricingSystem::add_instrument(Exchange exchangeForTicker, const std::string& ticker) {
    if (exchangeForTicker == Exchange::Kraken) {
        mTickersKraken.insert(ticker);
        mTickerToPriceKraken.insert({ticker, kSentinelPrice});
        return true;
    }
    return false;
}

} // namespace te

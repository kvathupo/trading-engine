#include "PricingSystem.hpp"

namespace te {

namespace {

// Dummy price held by each ticker until the exchange is queried
constexpr float kSentinelPrice{-1.0f};

} // namespace

bool PricingSystem::init() {
    return true;
}

bool PricingSystem::tick(const DataSystem& dataSystem) {
    // query the price on demand
    if (mExecution_mode != ExecutionMode::Backtest) {
        return true;
    }
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

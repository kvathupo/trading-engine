#include "PricingSystem.hpp"

#include "DataSystem.hpp"

namespace te {

bool PricingSystem::init(const ExecutionMode execution_mode) {
    mExecution_mode = execution_mode;
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

std::optional<float> PricingSystem::get_most_recent_price_for(Exchange exchange, std::string ticker) const {
    if (exchange == Exchange::Kraken) {
        auto itrToTickerPricePair = mTickerToPriceKraken.find(ticker);
        if (itrToTickerPricePair != mTickerToPriceKraken.end()) {
            return itrToTickerPricePair->second;
        }
    }
    return std::nullopt;
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
        return true;
    }
    return false;
}

} // namespace te

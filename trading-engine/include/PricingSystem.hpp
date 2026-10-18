#include <chrono>
#include <unordered_map>
#include <unordered_set>

#include <Types.hpp>

namespace te {

class DataSystem;

// @see DataParser
using Instrument = std::pair<std::string, float>;

/*
 *  Owns the initialization and update of price data.
 */
class PricingSystem {
public:
    bool init();

    /*
     *  Updates all price buffers from the data system if historical data.
     */
    bool tick(const DataSystem& dataSystem);
    void populateActiveTickers(const DataSystem& dataSystem);

    bool add_instrument(Exchange exchangeForTicker, const std::string& ticker);

    ExecutionMode mExecution_mode;

    // Kraken exchange
    std::unordered_set<std::string> mTickersKraken;
    std::unordered_map<std::string, float> mTickerToPriceKraken;
};

} // namespace te

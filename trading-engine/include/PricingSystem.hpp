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
    bool tick(const DataSystem& dataSystem, const TimePoint curr_time);

    /*
     *  When an instrument is added, its price data is tracked every frame.
     *
     *  @returns: False if the exchange is unsupported.
     */
    bool add_instrument(Exchange exchangeForTicker, const std::string& ticker);

    /*
     *  Returns the most recent price if present.
     *
     *  @param exchange Exchange for ticker
     *  @param ticker Ticker on exchange
     *
     *  @return
     *      - If historical data, populated optional if there's price data at
     *  the most recent sim time.
     *      - If live data, populated optional on exchange connectivity.
     */
    std::optional<float> get_most_recent_price_for(Exchange exchange, std::string ticker) const;

private:
    void populateActiveTickers(const DataSystem& dataSystem, const TimePoint curr_time);

    ExecutionMode mExecution_mode;

    // Kraken exchange
    std::unordered_set<std::string> mTickersKraken;
    std::unordered_map<std::string, float> mTickerToPriceKraken;
};

} // namespace te

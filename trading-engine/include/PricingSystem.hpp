#include <chrono>
#include <unordered_map>
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
     *  Updates all price buffers from the data system.
     */
    bool tick(const DataSystem& dataSystem);
    void populateActiveTickers(const DataSystem& dataSystem);

    ExecutionMode mExecution_mode;
    std::unordered_map<Exchange, std::vector<Instrument>> mExchangeToTickers;
};

}

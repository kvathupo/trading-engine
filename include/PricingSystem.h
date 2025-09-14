
#include <chrono>

/*
 *  Owns the initialization and update of price data.
 */
class PricingSystem {
public:
    /*
     *  Iterates over `./data-sources/` and initializes historical data feeds.
     *  Then iterates all data 
     */
    bool init();

    /*
     *  Updates all instruments
     */
    bool tick(const float deltaTime);

    std::vector<Instrument> instruments;
};

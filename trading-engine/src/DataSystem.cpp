#include "DataSystem.hpp"

namespace te {

    bool DataSystem::init(const ExecutionMode execution_mode, const std::vector<Exchange>& exchanges,
        const std::chrono::year_month_day start_date) {
        mExecution_mode = execution_mode;
        mExchanges = exchanges;
        return true;
    };

}   // end namespace te


#include "DataSystem.hpp"

#include <iostream>
#include <iterator>
#include <print>
#include <string_view>

namespace te {

    namespace {

    // @TODO(kvathupo): drop in favour of magic_enum once it is a dep
    constexpr std::string_view to_string(const Exchange exchange) {
        switch (exchange) {
            case Exchange::Kraken: return "Kraken";
        }
        return "Unknown";
    }

    }   // end anonymous namespace

    DataSystem::DataSystem(const ExecutionMode execution_mode):
        mExecution_mode{execution_mode}
    {};

    /*
     *  Seeks every historical parser up to `start_date`, dropping those whose data
     *  runs out before it. Live feeds are left untouched.
     */
    bool DataSystem::init(const std::chrono::year_month_day start_date) {
        if (mExchangeToDataSources.empty()) {
            std::println(std::cerr, "init failure: no data parsers registered");
            return false;
        }

        const std::chrono::sys_seconds start_time{std::chrono::sys_days{start_date}};
        for (auto& [exchange, parsers] : mExchangeToDataSources) {
            for (auto itr = parsers.begin(); itr != parsers.end(); ) {
                DataParser& parser = **itr;
                if (!parser.is_data_good()) {
                    std::println(std::cerr, "init failure: {} on {} has unusable data",
                        parser.get_ticker(), to_string(exchange));
                    return false;
                }

                // live feed?
                const auto tick_duration = parser.get_tick_duration();
                if (!tick_duration) {
                    ++itr;
                    continue;
                }

                // Update data parsers to the start time
                bool has_data_through_start{true};
                while (parser.get_newest_time() + *tick_duration <= start_time) {
                    if (!parser.tick()) {
                        std::println(std::cerr, "{} on {} lacked data through {}, or reached fatal error. Removing.",
                            parser.get_ticker(), to_string(exchange), start_date);
                        has_data_through_start = false;
                        break;
                    }
                }
                itr = has_data_through_start ? std::next(itr) : parsers.erase(itr);
            }
        }
        return true;
    }

    /*
     *  Advances every parser to the newest data point at or before `curr_time`.
     *  Running off the end of a feed is expected as a backtest progresses, so it is
     *  logged and the parser left in place. Only unusable data fails the tick.
     */
    bool DataSystem::tick(const std::chrono::time_point<std::chrono::system_clock,
            std::chrono::seconds>& curr_time) {
        bool parser_failure{false};
        for (auto& [exchange, parsers] : mExchangeToDataSources) {
            for (auto& parser_ptr : parsers) {
                DataParser& parser = *parser_ptr;

                // live feed?
                const auto tick_duration = parser.get_tick_duration();
                if (!tick_duration) {
                    continue;
                }

                // Advance while the earliest possible next data point still lands on or
                // before `curr_time`, so ticking never overshoots it.
                while (parser.get_newest_time() + *tick_duration <= curr_time) {
                    if (!parser.tick()) {
                        std::println("{} on {} exhausted its data at {}",
                            parser.get_ticker(), to_string(exchange), parser.get_newest_time());
                        break;
                    }
                }
            }
        }
        return !parser_failure;
    }

    bool DataSystem::add_data_parser(Exchange exchange, std::unique_ptr<DataParser> parser) {
        auto itrToExchange = mExchangeToDataSources.find(exchange);
        if (itrToExchange == mExchangeToDataSources.end()) {
            mExchangeToDataSources.insert({exchange, {}});
            itrToExchange = mExchangeToDataSources.find(exchange);
        }
        itrToExchange->second.push_back(std::move(parser));
        return true;
    }

}   // end namespace te

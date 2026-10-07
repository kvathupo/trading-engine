#include "DataSystem.hpp"

#include <iostream>
#include <print>
#include <string>
#include <string_view>

namespace te {

namespace {

// @TODO(kvathupo): drop in favour of magic_enum once it is a dep
constexpr std::string_view to_string(const Exchange exchange) {
    switch (exchange) {
    case Exchange::Kraken:
        return "Kraken";
    }
    return "Unknown";
}

} // end anonymous namespace

DataSystem::DataSystem(const ExecutionMode execution_mode) : mExecution_mode{execution_mode} {};

/*
 *  Seeks every historical parser up to `start_date`, dropping those whose data
 *  runs out before it. Live feeds are left untouched.
 */
bool DataSystem::init(const std::chrono::year_month_day start_date) {
    if (mTickerToKrakenParser.empty()) {
        std::println(std::cerr, "init failure: no data parsers registered");
        return false;
    }

    const std::chrono::sys_seconds start_time{std::chrono::sys_days{start_date}};
    for (auto itr = mTickerToKrakenParser.begin(); itr != mTickerToKrakenParser.end();) {
        const auto& [ticker, parser_ptr] = *itr;
        DataParser& parser = *parser_ptr;
        if (!parser.is_data_good()) {
            std::println(std::cerr, "init failure: {} on {} has unusable data", ticker, to_string(Exchange::Kraken));
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
                std::println(std::cerr, "{} on {} lacked data through {}, or reached fatal error. Removing.", ticker,
                             to_string(Exchange::Kraken), start_date);
                has_data_through_start = false;
                break;
            }
        }
        if (has_data_through_start) {
            ++itr;
        } else {
            itr = mTickerToKrakenParser.erase(itr);
        }
    }
    return true;
}

bool DataSystem::tick(const std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>& curr_time) {
    // With live data, the system is ticks independently on its own thread.
    // Ticking is done at its preconfigured tick rate.
    if (mExecution_mode != ExecutionMode::Backtest) {
        return true;
    }

    bool parser_failure{false};
    for (auto& [ticker, parser_ptr] : mTickerToKrakenParser) {
        DataParser& parser = *parser_ptr;

        // live feed?
        const auto tick_duration = parser.get_tick_duration();
        if (!tick_duration) {
            continue;
        }

        while (parser.get_newest_time() + *tick_duration <= curr_time) {
            if (!parser.tick()) {
                std::println("{} on {} exhausted its data at {}", ticker, to_string(Exchange::Kraken),
                             parser.get_newest_time());
                break;
            }
        }
    }
    return !parser_failure;
}

// @TODO(kvathupo): just get exchange from the parser
bool DataSystem::add_data_parser(Exchange exchange, std::unique_ptr<DataParser> parser) {
    if (exchange != Exchange::Kraken) {
        std::println(std::cerr, "add_data_parser failure: {} is not supported", to_string(exchange));
        return false;
    }
    mTickerToKrakenParser.insert({parser->get_ticker(), std::move(parser)});
    return true;
}

} // end namespace te

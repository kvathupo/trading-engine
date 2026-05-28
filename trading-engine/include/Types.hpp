#pragma once
/*
 *  Header of custom types and typedefs.
 *  Used for type-checking of template inputs.
 */

#include <concepts>
#include <cstdint>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>


/*
 *  Used to initialize a financial instrument from either a file or endpoint.
 */
namespace te {

template<typename T>
concept Arithmetic = std::is_arithmetic<T>::value;

using FilePath = std::string;

enum class InitializationType : std::uint_fast8_t {
    FileIo,
    WebSocket,
    MySql,
};

struct InitializationConfig {
    std::variant<FilePath> data;
    InitializationType type;
};

enum class ExecutionMode : std::uint_fast8_t {
    Backtest,       // historical data, simulated portfolios
    Paper,          // real-time data, simulated portfolios
    Live,           // real-time data, real portfolios
};

enum class Exchange : std::uint_fast8_t {
    Kraken,
};

enum class OrderBookSide : std::uint_fast8_t {
    Bid,
    Ask
};

}   // end `namespace te`

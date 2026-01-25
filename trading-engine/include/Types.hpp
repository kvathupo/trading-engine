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


/*
 *  Used to initialize a financial instrument from either a file or endpoint.
 */
namespace te {

template<typename T>
concept Arithmetic = std::is_arithmetic<T>::value;

union InitializationData {
    std::string path;
    std::string end_point;
};

enum class InitializationType : std::uint_fast8_t {
    FileIo,
    WebSocket,
    MySql,
};

struct InitializationConfig {
    InitializationData data;
    InitializationType type;
};

enum class Exchange : std::uint_fast8_t {
    Kraken,
};

enum class OrderBookSide : std::uint_fast8_t {
    Bid,
    Ask
};

}   // end `namespace te`

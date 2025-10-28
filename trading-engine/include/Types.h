/*
 *  Header of custom types and typedefs.
 *  Used for type-checking of template inputs.
 */

#include <concepts>
#include <optional>
#include <string>
#include <type_traits>


/*
 *  Used to initialize a financial instrument from either a file or endpoint.
 */
namespace te {

template<typename T>
concept Arithmetic = std::is_arithmetic<T>::value;

union InitializationConfig {
    std::string file_path;
};

}   // end `namespace te`

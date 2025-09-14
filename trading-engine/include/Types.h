/*
 *  Header of custom types and typedefs.
 *  Used for type-checking of template inputs.
 */

export module types;

import <concepts>
import <optional>


/*
 *  Used to initialize a financial instrument from either a file or endpoint.
 */
export {

namespace te {

union InitializationConfig {
    std::string file_path;
};

}   // end `namespace te`

}   // end `export` block

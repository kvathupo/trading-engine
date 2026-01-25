include_directories(include)

add_library(trading-engine STATIC)
target_sources(trading-engine
    PUBLIC
    FILE_SET core_systems TYPE HEADERS
    BASE_DIRS
    include
    FILES
    include/World.hpp
    include/DataParser.hpp
    include/KrakenParser.hpp

    PRIVATE
    src/World.cpp
    src/KrakenParser.cpp
)

add_subdirectory(tests)

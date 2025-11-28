include_directories(include)

add_library(trading-engine STATIC)
target_sources(trading-engine
    PUBLIC
    FILE_SET core_systems TYPE HEADERS
    BASE_DIRS
    include
    FILES
    include/World.hpp

    PRIVATE
    src/World.cpp
)

add_subdirectory(tests)

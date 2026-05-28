include(FetchContent)

# Hash to v1.17
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(include)

add_library(trading-engine STATIC)
target_sources(trading-engine
    PUBLIC
    FILE_SET core_systems TYPE HEADERS
    BASE_DIRS
    include
    FILES
    include/World.hpp
    include/DataSystem.hpp
    include/DataParser.hpp
    include/KrakenParser.hpp

    PRIVATE
    src/World.cpp
    src/DataSystem.cpp
    src/KrakenParser.cpp
)

# gtest_prod.h is included in production headers (for FRIEND_TEST), so
# the library needs the gtest include path. No linking required.
target_include_directories(trading-engine PRIVATE
    ${googletest_SOURCE_DIR}/googletest/include
)

add_subdirectory(tests)

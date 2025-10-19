include_directories(include)
include(FetchContent)

# Hash to v1.17
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

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

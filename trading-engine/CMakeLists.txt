include_directories(include)
include(FetchContent)
# Maybe not needed when I get a compatible g++ version?
#set(CMAKE_CXX_SCAN_FOR_MODULES ON)

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
    FILE_SET cxx_modules TYPE CXX_MODULES
    FILES
    include/World.ixx
    PRIVATE
    src/World.cpp
)

#
#   TODO:
#       - Use file sets
#


include(FetchContent)

# Hash to v1.17
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(run_tests.out World_tests.cpp)
target_link_libraries(run_tests.out PRIVATE
    gtest
    gtest_main

    trading-engine
)

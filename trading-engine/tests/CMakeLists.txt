add_executable(run_tests)

target_sources(run_tests
    PRIVATE
    KrakenParser_tests.cpp
    World_tests.cpp
)
target_link_libraries(run_tests PRIVATE
    gtest
    gtest_main

    trading-engine
)

target_compile_definitions(run_tests PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/"
)

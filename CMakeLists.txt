cmake_minimum_required(VERSION 4.00)
project(TradingEngine LANGUAGES CXX)

# Project-level compiler options
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

add_compile_options(-Wall -Wextra)

# Add libraries
add_subdirectory(trading-engine)

# Executable
add_executable(backtest.out
    src/main.cpp
)
target_link_libraries(backtest.out PRIVATE
    trading-engine
)

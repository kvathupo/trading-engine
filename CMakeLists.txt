cmake_minimum_required(VERSION 4.00)
project(TradingEngine LANGUAGES CXX)

# Project-level compiler options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)
set(GCM_CACHE_DIR ${CMAKE_SOURCE_DIR}/gcm.cache/gcm.cache)  # messed up symlink?

add_compile_options(-Wall -Wextra)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Clang detected")
    add_compile_options(
        -fmodules-cache-path=${GCM_CACHE_DIR}
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "GCC detected")
    add_compile_options(
        -fmodules-ts 
        -fmodule-mapper=|@g++-mapper-server\ -r${GCM_CACHE_DIR}
    )
endif()

# Add libraries
add_subdirectory(trading-engine)

# Executable
add_executable(backtest.out
    src/main.cpp
)
target_link_libraries(backtest.out PRIVATE
    trading-engine
)

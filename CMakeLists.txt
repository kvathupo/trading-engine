cmake_minimum_required(VERSION 4.00)
project(TradingEngine LANGUAGES CXX)

# Project-level compiler options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)
set(GCM_CACHE_DIR ${CMAKE_SOURCE_DIR}/gcm.cache/gcm.cache)  # messed up symlink?

# Populate gcm.cache with required standard library modules
set(REQUIRED_STD_MODULES
    "iostream"
    "chrono"
    "utility"
)
foreach(STD_MODULE IN LISTS REQUIRED_STD_MODULES)
    execute_process(
        COMMAND bash -c "find ${GCM_CACHE_DIR}/ -type f | grep '${STD_MODULE}'"
        RESULT_VARIABLE PIPE_EXIT_CODE
    )
    # TODO: make emojis proper. Might need to mess with terminal to do so
    # (copy-paste gave me these ugly ones)
    if(PIPE_EXIT_CODE EQUAL 0)
        message(STATUS "✅ Found '${STD_MODULE}'")
    else()
        message(STATUS "❌ '${STD_MODULE}' not found. Generating...")

        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            message(FATAL_ERROR "Clang support not yet added for module generation. "
                "Please generate ${STD_MODULE} manually"
            )
        elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            execute_process(
                COMMAND bash -c "${CMAKE_CXX_COMPILER} -std=c++20 -fmodules-ts -x \
                    c++-system-header ${STD_MODULE} -fmodule-mapper=\"|@g++-mapper-server \
                    -r${GCM_CACHE_DIR}\""
                RESULT_VARIABLE GEN_EXIT_CODE
            )
            if (GEN_EXIT_CODE EQUAL 1)
                message(FATAL_ERROR "Failed to generate missing module ${STD_MODULE}! "
                    "Please open issue and construct manually"
                )
            endif()
        endif()
    endif()
endforeach()

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

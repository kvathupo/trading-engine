cmake_minimum_required(VERSION 4.00)
project(TradingEngine LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

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

# Add a formatting target
find_program(CLANG_FORMAT_EXE NAMES clang-format-20 clang-format)
if(CLANG_FORMAT_EXE)
    execute_process(COMMAND ${CLANG_FORMAT_EXE} --version
        OUTPUT_VARIABLE CLANG_FORMAT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT CLANG_FORMAT_VERSION MATCHES "version 20\\.")
        message(WARNING "Expected clang-format 20, found: ${CLANG_FORMAT_VERSION}")
    endif()

    file(GLOB_RECURSE FORMAT_SOURCES CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/src/*.cpp
        ${PROJECT_SOURCE_DIR}/trading-engine/include/*.hpp
        ${PROJECT_SOURCE_DIR}/trading-engine/include/*.h
        ${PROJECT_SOURCE_DIR}/trading-engine/src/*.cpp
        ${PROJECT_SOURCE_DIR}/trading-engine/tests/*.cpp
    )
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i --style=file ${FORMAT_SOURCES}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Formatting C++ sources with clang-format"
        VERBATIM
    )
    # Really only important for CI gatekeeping
    add_custom_target(format-check
        COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror --style=file ${FORMAT_SOURCES}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Checking C++ formatting"
        VERBATIM
    )
else()
    message(STATUS "clang-format not found; 'format' targets unavailable")
endif()

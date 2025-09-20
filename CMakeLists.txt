cmake_minimum_required(VERSION 3.20)
project(TradingEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)
# Did I mess up the symlink?
set(GCM_CACHE_DIR ${CMAKE_SOURCE_DIR}/gcm.cache/gcm.cache)

add_compile_options(
    -fmodules-ts
    -fmodule-mapper="|@g++-mapper-server -r${GCM_CACHE_DIR}"
#   The following clang flag sets the gcm cache location. Possibly redundant
# with CMake however: see above!
#    -fmodules-cache-path=${GCM_CACHE_DIR}
)

add_executable(backtest.out
    src/main.cpp
)

#add_subdirectory(trading-engine)

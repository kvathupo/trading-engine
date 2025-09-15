# Roadmap
Immediate use-case: currency swaps
1. Write a basic example using historical data from Kraken
    - Hardcode the search paths for tickers. From a directory `./data-sources/`, have directories
      for individual tickers. Have directories contain CSVs.
    - Figure out logging
2. Worry about reproducibility: CMake, Conan, Docker
3. Write data feed on live data
# Developer Resources
## Prerequisites
* A C++20 compliant version of `g++`
* Ninja
* CMake

## Building
First, create a symlink to your global gcm cache:
```bash
MERLIN:~/repos/trading-engine$ mkdir gcm.cache
MERLIN:~/repos/trading-engine$ ln -fs ~/gcm.cache gcm.cache
```
Then, create an out-of-source build for cmake:
```bash
MERLIN:~/repos/trading-engine$ mkdir build
MERLIN:~/repos/trading-engine$ cd build
MERLIN:~/repos/trading-engine/build$ cmake .. -G Ninja
MERLIN:~/repos/trading-engine/build$ ninja -jn`proc`
```
## Development Guidelines
* Naming convention:
https://google.github.io/styleguide/cppguide.html
    - Use camelCase for functions instead
* Stuff to experiment with:
    - Concepts and Constraints
* How to compile standard library [modules](https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Modules.html):
```bash
g++ -std=c++20 -fmodules-ts -x c++-system-header string iostream -fmodule-mapper="|@g++-mapper-server -r./gcm.cache"
```
    * I'm not sure if absolute pathing is supported or if I'm on an old gcc version? Whenever I 
attempt to use absolute pathing, a single-quotation mark is prepended to my input `gcm.cache`, and
a subdirectory is generated from wherever I run the command, e.g. `'~` most annoyingly!
    * Compiling multiple modules is finnicky. I get code errors, presumably from the mapper service
program, but the modules are still generated into the cache!
* How to compile code importing modules:
```bash
g++ -std=c++20 -fmodules-ts test.cpp -fmodule-mapper="|@g++-mapper-server -r./gcm.cache"
```

# Resources
* Great answer, see the U alberta resource for assumptions, and how to model an exchange matching
engine, which spits out quotes, trades, and orders:
    https://quant.stackexchange.com/a/73841
## Data providers:
* Norgate:
    - https://norgatedata.com/stockmarketpackages.php

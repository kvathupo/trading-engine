# Developer Resources
* How to compile (until I setup CMake):
```bash
g++ -fmodules-ts -std=c++20 -Wall ./src/main.cpp -I include/
```
* Naming convention:
https://google.github.io/styleguide/cppguide.html
    - Use camelCase for functions instead
* Stuff to experiment with:
    - Concepts and Constraints
* How to compile a [module](https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Modules.html):
```bash
g++ -fmodules-ts -x c++-system-header iostream
```
# Roadmap
Immediate use-case: currency swaps
1. Write a basic example using historical data from Kraken
    - Hardcode the search paths for tickers. From a directory `./data-sources/`, have directories
      for individual tickers. Have directories contain CSVs.
    - Figure out logging
2. Worry about reproducibility: CMake, Conan, Docker
3. Write data feed on live data

# Resources
* Great answer, see the U alberta resource for assumptions, and how to model an exchange matching
engine, which spits out quotes, trades, and orders:
    https://quant.stackexchange.com/a/73841
## Data providers:
* Norgate:
    - https://norgatedata.com/stockmarketpackages.php

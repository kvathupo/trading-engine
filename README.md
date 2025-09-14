# System overview
Main loop:

Inputs to simulation:
    - tick rate, backtest start time, backtest end time
    - matching engine to use
1. World initializes
    i. Call pricing system init
        1. 
    ii. Call Portfolio system init
2. Until exit requested, world ticks
    i. Pricing system ticks
        Inputs: portfolio system
        1. For every exchange, for every asset,
            i. Update price, update order book
    ii. Matching engine ticks
        1. For all portfolios, iterate over orders. If order is invalid (insufficient
funds), then error. Else, mark filled or not.
    iii. Portfolio system ticks
        1. Read fills, and update balance and held instruments.
        2. Instrument values update (not needed if pointer to pricing system `curr_price` held)
    iv. Algorithm system ticks
    Inputs: Universe (global variable)
3. 
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

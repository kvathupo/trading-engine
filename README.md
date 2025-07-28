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
    ii. Portfolio system ticks
        1. Read fills, and update balance and held instruments.
        2. Instrument values update (not needed if pointer to pricing system `curr_price` held)
    iii. Algorithm system ticks
    Inputs: Universe (global variable)
3. 
# Roadmap
Immediate use-case: currency swaps
1. Write a basic example using historical data from Kraken
2. Worry about reproducibility: CMake, Conan, Docker
3. Write data feed on live data

# Resources
* Great answer, see the U alberta resource for assumptions, and how to model an exchange matching
engine, which spits out quotes, trades, and orders:
    https://quant.stackexchange.com/a/73841
## Data providers:
* Norgate:
    - https://norgatedata.com/stockmarketpackages.php

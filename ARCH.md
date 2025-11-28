# Architecture overview
## Core code
Inputs to simulation:
* tick rate, backtest start time, duration
* Portfolios
* matching engine to use
Outputs:
    - Logging System:
        * Plot of daily Pnl
        * Plot of fills
    - Statistics System?
        * Drawdown
        * Fills
        * Volatility

1. World initializes
    0. Init data system
        Inputs: unordered map from string to Data Parser type (has methods to get exchange, ticker, granularity, etc.)
    i. Call pricing system init
        1. Iterate over all CSV files.
        2. Populate data
        3. Find earliest date, and oldest date.
        4. Return true/false if data found
    ii. Call Portfolio system init
        1. Contains N Portfolios. Each has a balance, holdings, and a strategy (which issues orders to the matching engine).
2. Until exit requested, world ticks (like with unreal engine, don't have advance time [1])
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
    Inputs: Universe (global variable)
## Testing
* Eschew the use of `private` for `protected` in order to create shims that are exclusively
exported to gtest.
## Build Infra
* First, compile all module interface units. Shuffle them into the gcm cache.

[1] - https://www.youtube.com/watch?v=IaU2Hue-ApI

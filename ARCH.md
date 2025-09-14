# Architecture overview
Main loop:

Inputs to simulation:
* tick rate, backtest start time, duration
* matching engine to use

1. World initializes
    i. Call pricing system init
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

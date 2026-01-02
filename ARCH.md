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

0. World preinitialization -> to be replaced by reading in yaml file
    0. World adds data
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
        Inputs: Data System
        1. For every exchange, for every asset,
            i. Update price, update order book
    ii. Matching engine ticks
        Inputs: Pricing System, Portfolio System
        1. For all portfolios, iterate over orders. If order is invalid (insufficient
funds), then error. Else, mark filled or not. Allow querying "ANY" exchange, but warn if done.
    iii. Portfolio system ticks
        1. Read fills, and update balance and held instruments.
        2. Instrument values update (not needed if pointer to pricing system `curr_price` held)
    Inputs: Universe (global variable)
## Components
* Data parser
    - Role: For a given data source (e.g. CSV, websocket, etc.), get asset info. Handle I/O and data corruption.
    - Member Vars:
        - Buffer of price
        - Possibly buffer of time or just curr_time, start_time, end_time
    - Member functions (override these abstract ones):
* Data System
    - Members vars:
        - Hash map from <exchange,ticker> string to vector queue of unique pointers of data parser instances. The 
        front of the queue represents the oldest data. As the sim progresses, delete unneeded data.
    - Member funcs:
        - Get price
            - Given ticker and exchange as strings, just call hash map
            - Given ticker, iterate over all exchanges
## Testing
* Eschew the use of `private` for `protected` in order to create shims that are exclusively
exported to gtest.
## Build Infra
* First, compile all module interface units. Shuffle them into the gcm cache.

## Commentary
### Comparisons to game and defense simulations
#### Often changed inputs
* At startup, a game is pointed to a collection of binary files, each of which 
references other binary files. A subset are pulled in at initialization (defined
as loading a map), and others are gradually pulled in on a as-needed basis.
    * In Unreal Engine, assets are held in the `Content` dir. Default maps,
target hardware, and metadata are held in the `Config` dir. The latter are 
loaded at game init. UE provides a GUI to populate the `Config` fields on 
fresh project start.
    * UE knows how to load assets and treat them in relation to other assets
since they are binary files authored within the editor. The UE-specific
extension (originating in UE via its GUI) dictates this.
#### Not often changes inputs (extension points)
* Extention code is placed in a particular directory. It is either compiled 
statically or dynamically against the core codebase.
    * In Unreal Engine at a AAA studio, we compiled dynamically against UE via
DLLs.

[1] - https://www.youtube.com/watch?v=IaU2Hue-ApI

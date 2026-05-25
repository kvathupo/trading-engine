# Architecture overview
## Core code
Inputs to simulation:
* tick rate, backtest start time, duration
* Portfolios
    - Member vars mutated by simulation:
        - Balance, holdings, pending fills
    - Members vars not mutated:
        - Exchange
    - Callback processed each tick
      - Inputs:
          - Asset universe
                - Assets have a ticker, type, most recent price, time, and order book
                - Methods:
                    - Get all assets in universe
                    - Get all assets by type
          - Failed fills from last tick
          - Pending fills
      - Outputs:
          - Order requests

Outputs:
    - Logging System:
        * Plot of daily Pnl
        * Plot of fills
    - Statistics System?
        * Drawdown
        * Fills
        * Volatility

## User experience
1. In the ctor of World, the user specifies a list of exchange enums, and a `LIVE` or `HISTORICAL` enum
2. User attaches a collection of portfolios
3. User calls `run_backtest()` on the World
4. Evaluates the results.

0. World preinitialization
    - Ctor and portfolio attachment is expected
1. World initializes
    0. Init data system
        - Knows for a particular exchange and live/historical enum, which parser to use
        Inputs: list of exchange enums and whether to use live or historical data

        1. Reads configuration file mapping exchange to historical data location
        2. Using internal mapping from (exchange, feed_type) to Parser type, initializes a parser for each
        asset in that directory. Stores a collection of assets (they hold exchange type), 
    i. Call pricing system init
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
    - Role: Given data parsers of varying granularity, return the current price of an asset (given as a string
      exchange, ticker pair)
    - Members vars:
        - Hash map from <exchange,ticker> string to vector queue of unique pointers of data parser instances. The 
        front of the queue represents the oldest data. As the sim progresses, delete unneeded data.
    - Member funcs:
        - Get price
            - Given ticker and exchange as strings, just call hash map
            - Given ticker, iterate over all exchanges
* Price System
    - Role: 
        - Get a list of all assets in a type-safe manner (Asset `class` with instrument type `enum` and ticket 
          `string`)
            - Get a list of all assets on an exchange
        - Given a type-safe Asset class, return all possible prices with fees per liquidity source (can be multiple
          per exchange if DEX)
    - Member funcs:
        - Get price
## Testing
* Eschew the use of `private` for `protected` in order to create shims that are exclusively
exported to gtest.

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

# Architecture overview
https://x.com/BrettHarrison/status/2080297166229094480

Data tick (informs prices and fills)
    Design decision: Send orders and read fills to (1) conceptually have a notion of trading client tick rate
    separate from exchange server tick rate and (2) make code easier to reason about by encapsulation.

    Update server-side, portfolio-specific data (to prioritize sending orders on most recent data):
    1. Iterate over all order requests, and send to exchange async. 
    Update server-side, portfolio-agnostic data:
    1. Iterate over all exchange, ticker pairs (data parser), update price
    Update server-side, portfolio-specific data:
    1. Read fills from earlier. Write if accepted or rejected by exchange.
Price tick (so portfolio decisions made on most recent values),
    - Some degree of slippage from ground truth asset prices from network latency
Portfolio system tick:
    - Server side (for each portfolio, vector of portfolio class instances): 
        - Prices update for portfolio assets held and next orders requests (from the client side tick)
        - Order requests last tick turned into response variable or Order:     (map from order request id to request and status)
            For all order requests,
            - If status is rejected, remove. These are orders that were rejected previous tick, and carried over.
            - Bid/ask request:
                i. If accepted by exchange,
                    1. If filled, make changes to portfolio assets and account balance
                    2. If not filled, add to portfolio's pending orders (map from order id to OrderInfo and status)
                ii. If rejected, update status
            - Cancel request.
                i. If accepted by exchange, 
                    1. Remove from pending orders (map from order id to OrderInfo and status).
                    If bid cancelled, return funds. If ask cancelled, deregister those shares as locked up
                    (from a portfolio's pending asks list of `asset, quantity` pairs, remove it).
                ii. If rejected, update portfolio. 
        - Filled orders turned into portfolio changes
    - Client side (for each portfolio):
        - Order requests are made
## Real world
### Client
1. User makes request (Portfolio system)
    i. Bid
        1. Accept if sufficient funds
        2. Reject if insufficient funds
    ii. Ask
        1. Accept if sufficient assets (assets in portfolio, minus pending asks)
        2. Reject if inufficient assets
    iii. Cancel
        1. Accept if order exists (requires tracking pending orders)
        2. Reject if order does not exist
2. Next tick,
    i. Exchange level info updated:
        1. Order book.
    ii. Portfolio level info updated:
        i. For each request,
            1. If accepted by exchange, becomes order. Add id and description.
        i. For each order in the portfolio, the status is updated:
            1. Bid/ask filled
            2. Sits
            3. Cancelled
        ii. Portfolio holdings update
        ii. Portfolio changes value
### Server
1. Order book exists with bid/ask. Liquidity pool is just order book size one. (this tick)
2. Orders are received. Cancellations are received.
3. Matching engine runs. 
4. Fills are sent out. Succesfull cancellations are sent out. (next tick)
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
        Inputs: Data system

        1. Initializes a list of `Asset` instances. An `Asset` class contains an exchange enum, string ticker,
        and `AssetType` variant/enum/struct (e.g. a swap has two members). These assets are generated from all 
        active assets in the Data System.
        2. Initialies a `Universe` class, which is passed to the portfolio system, which passes it to the callbacks on
        individual portfolios. The `Universe` class queries the pricing system API for most up-to-date prices.
    ii. Call Portfolio system init
        Inputs: N Portfolios.

        1. 
    iii. Matching System init
        Inputs: MatchType 

        1. If Naive, fill order if above ask (resp. below bid). If OrderBook, check volume and guarantee fill.
        If OrderBookWithProbability, check volume and fill if binomial samping sufficient.
2. Until exit requested, world ticks with steady clock duration (like with unreal engine, don't have advance time [1])
    i. Data system ticks
        Inputs: time delta
    
        For each exchange, for each asset,
            1. If historical data, peek at the next available time for the asset. If strictly greater than 
            `curr_time+time_delta`, then tick the data parser.
            2. If live data, tick each parser.
    i. Pricing system ticks
        Inputs: Data System, time delta
        1. For every exchange, for every asset,
            i. Update price, update order book
    ii. Matching engine ticks
        Inputs: Pricing System, Portfolio System
        1. For all portfolios, iterate over orders. If order is invalid (insufficient
funds), then error. Else, mark filled or not. Allow querying "ANY" exchange, but warn if done.
    iii. Price System ticks
        1. Updates all asset prices from 
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
* Portfolio System
    - Member vars:
        - Vector of `{portfolio id, Portfolio}` pairs.
    - `Portfolio` class
        * Member vars:
            - Map from `OrderRequestId` to `OrderRequest`
            - Map from `OrderRequestId` to `OrderRequestStatus` enum (Accept, Reject, Sent)
            - Map from `OrderId` to `Order` 
            - Map from `OrderId` to `OrderStatus` enum (Pending, Filled, Cancelled, Completed)
            - `double balance`
            - Map from `string` ticker to `size_t` amount held
            - Map from `string` ticker to `size_t` amount locked up (collateral or pending order)
    - `Bid` struct: Ticker string, `size_t` amount
    - `Ask` struct: Ticker string, `size_t` amount
    - `Cancel` struct: `OrderId`
    - `OrderRequest` class
        * Member vars: Exchange string, `union` of Bid/Ask/Cancel
    - `Order` class
        * Member vars: Exchange string, `union` of Bid/Ask/Cancel
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

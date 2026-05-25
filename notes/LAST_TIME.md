# Last Time
* Write `KrakenParser` impl
    - Added test data for kraken swaps
        - tick -> DONE, Cleaned up
        - get new price
        - get time
        - get duration (might need to rethink interface)
        - get transaction fee
        - get order book
        - Replicate test for other data sets
    - Rename `validate_str_to_num` to something not retarded 
    - Handle TODOs, except with magic enum
* Data System role in relation to Pricing system (subsume the two?)
* Data System API
* Data Parser implementation
* Data System implementtation
* Pricing System

* Set up LSP with vim
    - https://copilot.microsoft.com/shares/nxzwYu8DM1d7EnqKiVfW8
    - https://jonasdevlieghere.com/post/vim-lsp-clangd/
* Set up linter
* Clean up CMake setup
    - Kraken Parser links against a shared trading
library
    - Test executable is the build folder's top level

# What not to worry about for now
In order of decreasing relevance:
* Running metabacktests
    * Intraday timing granularity
* Order book
* Matching Engine
    * Assume perfect for now
* Passing in fixed buffer to world init would be fun

# Possible Utilities
* https://github.com/fmtlib/fmt

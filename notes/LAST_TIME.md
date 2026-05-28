# Last Time
* Write Data System (See `Arch.md`)
    - Writing out `tick()` logic in `Arch.md`. -> CURR - vibe-code linter and yaml parser for ctor
        - Unknowns:
            - Should the `Universe` hold anything, other than refs to systems?
            - How to handle live trading in all systems
    - Writing ctor
        - Want to make configurable in yaml:
            - Data for a given exchange is in one directory, which contains all assets of differing
              granularites. Deconflict the assets by prefix string. Deconflict granularities with suffix
              string.
        - Mapping from exchange to directory if historical data.
            - Require same granularity for now. Take the one big CSV approach.
        - Mapping from exchange to endpoint with auth keys (DANGER!) if live.
    - Subsume into Price system?
* Data System API
* Data Parser implementation
* Data System implementation
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

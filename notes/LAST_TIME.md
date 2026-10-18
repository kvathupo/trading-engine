# Last Time
* Thinking through design of client side. See `Arch.md`. -> DONE
* Doing the Make orders work flow -> CURR
    - Merge in branch with feed handler getting prices from data system
        i. Implement `KrakenParser::has_next_price()` -> DONE
        ii. Implement `DataSystem::get_newest_price()` ->  DONE
        iii. Implement `PricingSystem::tick()` -> DONE



* Do camelcase and snake case shit
    - Check conventions I use in linter (google vs Clang)
* Make ECS 
    - hash maps into vectors ordered by id
        - Log(n) insertion and search
    - Why maintain maps internally? Just have users do that and send back all the shit.

* Set up LSP with vim
    - https://copilot.microsoft.com/shares/nxzwYu8DM1d7EnqKiVfW8
    - https://jonasdevlieghere.com/post/vim-lsp-clangd/
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

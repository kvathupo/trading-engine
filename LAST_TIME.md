# Last Time
* Building g++14 (earliest version compatible with module scanning, which is used to 
solve the problem of module import order, see 
https://www.youtube.com/watch?v=7WK42YSfE9s)
    - GCC PPAs and ubuntu's universe repo don't have gcc 14 on 22.04...
* Move World to be a module
* Set up gtest
* Write World timing test
* Set up linter

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

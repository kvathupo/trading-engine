# Last Time
* Setting up CMake
    - Running into issue where cmake is not finding the gcm cache. Need to bing
for a cmake lists file that uses gcm caches
    - https://stackoverflow.com/questions/57300495/how-to-use-c20-modules-with-cmake
    - target_compile_features
* Set up gtest
* Write World timing test

# What not to worry about for now
In order of decreasing relevance:
* Running metabacktests
    * Intraday timing granularity
* Order book
* Matching Engine
    * Assume perfect for now
* Passing in fixed buffer to world init would be fun

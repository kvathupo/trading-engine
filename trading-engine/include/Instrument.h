#include <concepts>
#include <chrono>

#include <Types.h>

template<std::floating_point T> 
class Instrument {
public:

    init(InitializationConfig cfg);

    std::chrono::time_point<std::chrono::system_clock> peekTime() const;
    T peekPrice() const;

    T curr_price;
    std::chrono::time_point<std::chrono::system_clock> curr_time;
};

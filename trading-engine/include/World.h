#include <chrono>
#include <utility>

namespace te {

class World {
public:
    bool init(const std::chrono::year_moth_day& start,
        const std::chrono::years& duration);

    bool tick(const std::chrono::day& deltaTime);

private:
    const std::chrono::year_moth_day start,
    const std::chrono::years duration;
};

}

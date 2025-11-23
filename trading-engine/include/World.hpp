#include <Types.h>

#include <chrono>
#include <utility>

namespace te {

class World {
public:
    World() = default;
    bool init(const std::chrono::year_month_day& start,
        const std::chrono::years& duration);
    bool tick(const std::chrono::duration<double>& deltaTime);

private:
    std::optional<std::chrono::year_month_day> start = {};
    std::optional<std::chrono::years> duration = {};
};

}

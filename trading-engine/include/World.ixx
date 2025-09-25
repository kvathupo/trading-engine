export module te.World

import <chrono>;
import <utility>;

export namespace te {

class World {
public:
    bool init(const std::chrono::year_month_day& start,
        const std::chrono::years& duration);
    bool tick(const std::chrono::day& deltaTime);

private:
    std::optional<std::chrono::year_month_day> start,
    std::optional<std::chrono::years> duration;
};

}

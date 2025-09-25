module te.World

bool te::World::init(const std::chrono::year_month_day& start,
        const std::chrono::years& duration) {
    return true;
}

bool te::World::tick(const std::chrono::day& deltaTime) {
    return true;
}

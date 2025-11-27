#include <Types.h>

#include <chrono>
#include <utility>

namespace te {

class World {
public:
    World() = default;
    bool init(const std::chrono::year_month_day& start,
        const std::size_t& num_days);
    bool tick(const std::chrono::duration<double>& deltaTime);

/*  
 *  @TODO(kvathupo): Initialize DataParser pointing to that path
 *  bool add_data(const std::string& path_to_dir, std::shared_ptr<DataParser> parser)
 *
 */
private:
    std::optional<std::chrono::year_month_day> start = {};
    std::optional<std::chrono::minutes> duration = {};
    // @TODO(kvathupo): Move to dedicated header to not repeat time defs in datasystem, etc.
    std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes> curr_time;
};

}

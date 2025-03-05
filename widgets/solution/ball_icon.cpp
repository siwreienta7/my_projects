#include "ball_icon.hpp"
#include <utility>

namespace widgets {
[[nodiscard]] widget *ball_icon::child_at(int x, int y) {
    const int dx = radius() - x;
    const int dy = radius() - y;
    if (dx * dx + dy * dy <= radius() * radius()) {
        return this;
    }
    return nullptr;
}

ball_icon::ball_icon(int radius_) : m_radius(radius_) {
}

[[nodiscard]] std::unique_ptr<ball_icon> make_ball_icon(int radius) {
    return std::make_unique<ball_icon>(radius);
}
}  // namespace widgets
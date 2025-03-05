#ifndef BALL_ICON_HPP_
#define BALL_ICON_HPP_

#include <memory>
#include "abstract_widgets.hpp"

namespace widgets {
struct ball_icon final : widget {
    [[nodiscard]] int radius() const {
        return m_radius;
    }

    void radius(int new_radius) {
        m_radius = new_radius;
    }

    [[nodiscard]] int width() const final {
        return m_radius * 2 + 1;
    }

    [[nodiscard]] int height() const final {
        return m_radius * 2 + 1;
    }

    [[nodiscard]] widget *child_at(int x, int y) override;

    explicit ball_icon(int radius);

private:
    int m_radius;
};

[[nodiscard]] std::unique_ptr<ball_icon> make_ball_icon(int radius);
}  // namespace widgets

#endif  // BALL_ICON_HPP_

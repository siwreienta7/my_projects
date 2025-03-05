#ifndef PLACEHOLDER_HPP_
#define PLACEHOLDER_HPP_

#include <memory>
#include "abstract_widgets.hpp"

namespace widgets {
struct placeholder : widget {
    [[nodiscard]] int width() const final {
        return m_width;
    }

    [[nodiscard]] int height() const final {
        return m_height;
    }

    explicit placeholder(int width_, int height_);

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes)
    int m_width;
    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes)
    int m_height;
};

[[nodiscard]] std::unique_ptr<placeholder>
make_placeholder(int width, int height);
}  // namespace widgets

#endif  // PLACEHOLDER_HPP_

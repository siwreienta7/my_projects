#include "abstract_widgets.hpp"

namespace widgets {
[[nodiscard]] widget *widget::child_at(int x, int y) {
    if (0 <= x && x < width() && 0 <= y && y < height()) {
        return this;
    }
    return nullptr;
}

}  // namespace widgets

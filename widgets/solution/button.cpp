#include "button.hpp"
#include <utility>

namespace widgets {
[[nodiscard]] int button::width() const {
    return 10 + 8 * static_cast<int>(m_label.length());
}

[[nodiscard]] int button::height() const {
    return 10 + 12;
}

button::button(std::string label) : m_label(std::move(label)) {
}

[[nodiscard]] std::unique_ptr<button> make_button(std::string label) {
    return std::make_unique<button>(std::move(label));
}
}  // namespace widgets

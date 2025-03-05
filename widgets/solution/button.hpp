#ifndef BUTTON_HPP_
#define BUTTON_HPP_

#include <memory>
#include <string>
#include "abstract_widgets.hpp"

namespace widgets {
struct button final : widget {
    [[nodiscard]] int width() const final;
    [[nodiscard]] int height() const final;

    explicit button(std::string label);

    [[nodiscard]] const std::string &label() const {
        return m_label;
    }

    void label(std::string new_label) {
        m_label = std::move(new_label);
    }

private:
    std::string m_label;
};

[[nodiscard]] std::unique_ptr<button> make_button(std::string label);
}  // namespace widgets

#endif  // BUTTON_HPP_

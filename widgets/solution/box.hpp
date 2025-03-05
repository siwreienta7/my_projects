#ifndef BOX_HPP_
#define BOX_HPP_

#include <memory>
#include <utility>
#include <vector>
#include "abstract_widgets.hpp"

namespace widgets {
struct box final : container {
    enum class kind { HORIZONTAL, VERTICAL };

    explicit box(kind form);

    [[nodiscard]] int size() const;
    [[nodiscard]] widget *get(int index) const;
    widget *add(std::unique_ptr<widget> child);
    std::unique_ptr<widget> remove(int index);

    [[nodiscard]] widget *child_at(int x, int y) override;

    void update_layout() override;

private:
    kind m_form;
    std::vector<std::unique_ptr<widget>> m_children;
};

bool check_correctness_of_coord(int coord, std::pair<int, int> coord_borders);

[[nodiscard]] std::unique_ptr<box> make_box(box::kind form);
}  // namespace widgets

#endif
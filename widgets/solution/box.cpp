#include "box.hpp"
#include <numeric>

namespace widgets {
box::box(box::kind form_) : m_form(form_) {
}

int box::size() const {
    return static_cast<int>(m_children.size());
}

widget *box::get(const int index) const {
    if (index >= size() || index < 0) {
        return nullptr;
    }
    return m_children[index].get();
}

widget *box::add(std::unique_ptr<widget> child_) {
    m_children.push_back(std::move(child_));
    set_parent(this, m_children[size() - 1].get());
    if (m_form == box::kind::HORIZONTAL) {
        set_width(width() + m_children[size() - 1]->width());
        set_height(std::max(height(), m_children[size() - 1]->height()));
    } else {
        set_height(height() + m_children[size() - 1]->height());
        set_width(std::max(width(), m_children[size() - 1]->width()));
    }
    return m_children[size() - 1].get();
}

std::unique_ptr<widget> box::remove(const int index) {
    if (index >= size() || index < 0) {
        return nullptr;
    }
    std::unique_ptr<widget> temp_widget = std::move(m_children[index]);
    if (temp_widget) {
        set_parent(nullptr, temp_widget.get());
    }
    m_children.erase(m_children.begin() + index);
    update_layout();
    return temp_widget;
}

bool check_correctness_of_coord(int coord, std::pair<int, int> coord_borders) {
    return (coord >= coord_borders.first && coord <= coord_borders.second);
}

[[nodiscard]] widget *box::child_at(int x, int y) {
    if (!check_correctness_of_coord(x, {0, width() - 1}) ||
        !check_correctness_of_coord(y, {0, height() - 1})) {
        return nullptr;
    }

    int current_x = 0;
    int current_y = 0;
    for (auto &child : m_children) {
        const int child_width = child->width();
        const int child_height = child->height();
        std::pair<int, int> borders_for_x;
        std::pair<int, int> borders_for_y;
        if (m_form == box::kind::HORIZONTAL) {
            borders_for_x = {current_x, current_x + child_width - 1};
            borders_for_y = {
                (height() - child_height) / 2, (height() + child_height) / 2};
        } else {
            borders_for_x = {
                (width() - child_width) / 2, (width() + child_width) / 2};
            borders_for_y = {current_y, current_y + child_height - 1};
        }
        if (check_correctness_of_coord(x, borders_for_x) &&
            check_correctness_of_coord(y, borders_for_y)) {
            return child->child_at(
                x - borders_for_x.first, y - borders_for_y.first
            );
        }
        current_x += child_width;
        current_y += child_height;
        if ((x < current_x && m_form == box::kind::HORIZONTAL) ||
            (y < current_y && m_form == box::kind::VERTICAL)) {
            return nullptr;
        }
    }
    return nullptr;
}

void box::update_layout() {
    int current_height = 0;
    int current_width = 0;
    if (m_form == box::kind::HORIZONTAL) {
        current_width = std::accumulate(
            m_children.begin(), m_children.end(), 0,
            [](int cur_w, const auto &child) { return cur_w + child->width(); }
        );
        for (const auto &child : m_children) {
            current_height = std::max(current_height, child->height());
        }
    } else {
        current_height = std::accumulate(
            m_children.begin(), m_children.end(), 0,
            [](int cur_h, const auto &child) { return cur_h + child->height(); }
        );
        for (const auto &child : m_children) {
            current_width = std::max(current_width, child->width());
        }
    }
    set_height(current_height);
    set_width(current_width);
}

[[nodiscard]] std::unique_ptr<box> make_box(box::kind form) {
    return std::make_unique<box>(form);
}

}  // namespace widgets
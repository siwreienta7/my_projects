#include "grid.hpp"
#include <numeric>

namespace widgets {

grid::grid(int rows_, int columns_) : m_rows(rows_), m_columns(columns_) {
    m_heights.resize(rows_);
    m_widths.resize(columns_);
    m_children.resize(rows_);
    for (int i = 0; i < rows_; i++) {
        m_children[i] = std::vector<std::unique_ptr<widget>>(columns_);
    }
}

int grid::rows() const {
    return m_rows;
}

int grid::columns() const {
    return m_columns;
}

widget *grid::get(int row_, int column_) const {
    if (row_ < 0 || column_ < 0 || row_ >= rows() || column_ >= columns()) {
        return nullptr;
    }
    return m_children[row_][column_].get();
}

widget *grid::add(std::unique_ptr<widget> child_, int row_, int column_) {
    if (row_ < 0 || column_ < 0 || row_ >= rows() || column_ >= columns()) {
        return nullptr;
    }
    if (m_children[row_][column_]) {
        set_parent(nullptr, m_children[row_][column_].get());
    }
    m_children[row_][column_] = std::move(child_);
    reset_heights_and_widths(row_, column_);
    set_parent(this, m_children[row_][column_].get());
    return m_children[row_][column_].get();
}

std::unique_ptr<widget> grid::remove(int row_, int column_) {
    if (row_ < 0 || column_ < 0 || row_ >= rows() || column_ >= columns()) {
        return nullptr;
    }
    std::unique_ptr<widget> temp_widget = std::move(m_children[row_][column_]);
    if (temp_widget) {
        set_parent(nullptr, temp_widget.get());
    }
    reset_heights_and_widths(row_, column_);
    return temp_widget;
}

[[nodiscard]] widget *grid::child_at(int x, int y) {
    if (x < 0 || y < 0 || x >= width() || y >= height()) {
        return nullptr;
    }
    int current_row = -1;
    int current_column = -1;
    int current_width = 0;
    int current_height = 0;
    for (int i = 0; i < m_columns; i++) {
        if (m_widths[i] == 0) {
            continue;
        }
        if (x >= current_width && x < current_width + m_widths[i]) {
            current_column = i;
            break;
        }
        current_width += m_widths[i];
    }
    for (int i = 0; i < m_rows; i++) {
        if (m_heights[i] == 0) {
            continue;
        }
        if (y >= current_height && y < current_height + m_heights[i]) {
            current_row = i;
            break;
        }
        current_height += m_heights[i];
    }
    if (current_row == -1 || current_column == -1 ||
        !m_children[current_row][current_column]) {
        return nullptr;
    }
    if (x < current_width + m_children[current_row][current_column]->width() &&
        y < current_height +
                m_children[current_row][current_column]->height()) {
        return m_children[current_row][current_column]->child_at(
            x - current_width, y - current_height
        );
    }
    return nullptr;
}

void grid::reset_heights_and_widths(int row_, int column_) {
    int current_width = 0;
    int current_height = 0;
    for (int i = 0; i < rows(); i++) {
        if (!m_children[i][column_]) {
            continue;
        }
        current_width =
            std::max(current_width, m_children[i][column_]->width());
    }
    for (int i = 0; i < columns(); i++) {
        if (!m_children[row_][i]) {
            continue;
        }
        current_height =
            std::max(current_height, m_children[row_][i]->height());
    }
    set_height(height() - m_heights[row_] + current_height);
    m_heights[row_] = current_height;
    set_width(width() - m_widths[column_] + current_width);
    m_widths[column_] = current_width;
}

void grid::update_layout() {
    std::fill(m_heights.begin(), m_heights.end(), 0);
    std::fill(m_widths.begin(), m_widths.end(), 0);
    for (int i = 0; i < rows(); i++) {
        for (int j = 0; j < columns(); j++) {
            if (!m_children[i][j]) {
                continue;
            }
            m_heights[i] = std::max(m_heights[i], m_children[i][j]->height());
            m_widths[j] = std::max(m_widths[j], m_children[i][j]->width());
        }
    }
    set_width(std::accumulate(m_widths.begin(), m_widths.end(), 0));
    set_height(std::accumulate(m_heights.begin(), m_heights.end(), 0));
}

[[nodiscard]] std::unique_ptr<grid> make_grid(int rows, int columns) {
    return std::make_unique<grid>(rows, columns);
}
}  // namespace widgets
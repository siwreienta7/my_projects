#ifndef GRID_HPP_
#define GRID_HPP_

#include <memory>
#include <vector>
#include "abstract_widgets.hpp"

namespace widgets {
struct grid final : container {
    explicit grid(int rows, int columns);

    [[nodiscard]] int rows() const;
    [[nodiscard]] int columns() const;
    [[nodiscard]] widget *get(int row, int column) const;
    widget *add(std::unique_ptr<widget> child, int row, int column);
    std::unique_ptr<widget> remove(int row, int column);
    [[nodiscard]] widget *child_at(int x, int y) override;
    void reset_heights_and_widths(int row, int column);
    void update_layout() override;

private:
    int m_rows = 0;
    int m_columns = 0;
    std::vector<std::vector<std::unique_ptr<widget>>> m_children;
    std::vector<int> m_heights;
    std::vector<int> m_widths;
};

[[nodiscard]] std::unique_ptr<grid> make_grid(int rows, int columns);
}  // namespace widgets

#endif

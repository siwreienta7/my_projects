#include "matrix.hpp"
#include <stdexcept>
#include <vector>

namespace matrix_interpreter {
void Matrix::check_mismatch(const int lhs_parameter, const int rhs_parameter) {
    if (lhs_parameter != rhs_parameter) {
        throw matrix_size_mismatch(lhs_parameter, rhs_parameter);
    }
}

Matrix::Matrix(const int rows, const int columns) noexcept
    : m_rows(rows),
      m_columns(columns),
      m_elements(rows, std::vector<int>(columns)) {
}

void Matrix::set_matrix_element(
    const int row,
    const int column,
    const int value
) noexcept {
    m_elements[row][column] = value;
}

[[nodiscard]] int Matrix::get_matrix_element(const int row, const int column)
    const {
    return m_elements.at(row).at(column);
}

[[nodiscard]] int Matrix::rows() const noexcept {
    return m_rows;
}

[[nodiscard]] int Matrix::columns() const noexcept {
    return m_columns;
}

Matrix &Matrix::operator+=(const Matrix &rhs) {
    check_mismatch(rows(), rhs.rows());
    check_mismatch(columns(), rhs.columns());
    for (int i = 0; i < rows(); i++) {
        for (int j = 0; j < columns(); j++) {
            m_elements.at(i).at(j) += rhs.get_matrix_element(i, j);
        }
    }
    return *this;
}

Matrix &Matrix::operator*=(const Matrix &rhs) {
    check_mismatch(columns(), rhs.rows());
    std::vector<std::vector<int>> new_matrix(
        rows(), std::vector<int>(rhs.columns())
    );
    for (int i = 0; i < rows(); i++) {
        for (int j = 0; j < rhs.columns(); j++) {
            long long current_elem = 0;
            for (int k = 0; k < rhs.rows(); k++) {
                current_elem +=
                    static_cast<long long>(get_matrix_element(i, k)) *
                    static_cast<long long>(rhs.get_matrix_element(k, j));
            }
            new_matrix.at(i).at(j) = static_cast<int>(current_elem);
        }
    }
    m_elements = std::move(new_matrix);
    m_columns = rhs.columns();
    return *this;
}
}  // namespace matrix_interpreter

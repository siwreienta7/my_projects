#ifndef MATRIX_HPP_
#define MATRIX_HPP_

#include <stdexcept>
#include <string>
#include <vector>

namespace matrix_interpreter {
struct matrix_size_mismatch : std::exception {
    matrix_size_mismatch(int lhs_parameter, int rhs_parameter)
        : message(
              "Dimension mismatch: lhs=" + std::to_string(lhs_parameter) +
              ", rhs=" + std::to_string(rhs_parameter)
          ) {
    }

    [[nodiscard]] const char *what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

struct Matrix {
    Matrix(int rows, int columns) noexcept;
    void set_matrix_element(int row, int column, int value) noexcept;
    [[nodiscard]] int get_matrix_element(int row, int column) const;
    [[nodiscard]] int rows() const noexcept;
    [[nodiscard]] int columns() const noexcept;

    static void check_mismatch(int lhs_parameter, int rhs_parameter);

    Matrix &operator+=(const Matrix &rhs);
    Matrix &operator*=(const Matrix &rhs);

private:
    int m_rows;
    int m_columns;
    std::vector<std::vector<int>> m_elements;
};
}  // namespace matrix_interpreter

#endif

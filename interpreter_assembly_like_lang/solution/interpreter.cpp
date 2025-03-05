#include "interpreter.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace matrix_interpreter {
std::pair<Command, std::vector<std::string>> parse_commands(
    const std::string &command
) noexcept {
    std::istringstream in_stream(command);
    std::string token;
    std::vector<std::string> tokens_to_return;
    while (in_stream >> token) {
        tokens_to_return.push_back(token);
    }
    if (tokens_to_return.empty()) {
        return {Command::NOTHING_TO_DO, tokens_to_return};
    }
    if (INPUT_TO_COMMANDS.contains(tokens_to_return[0])) {
        return {INPUT_TO_COMMANDS.at(tokens_to_return[0]), tokens_to_return};
    }
    return {Command::UNKNOWN, tokens_to_return};
}

void Interpreter::load(const int register_index, const std::string &file) {
    std::ifstream file_input(file);
    if (!file_input) {
        throw std::runtime_error("Unable to open file '" + file + "'");
    }
    file_input.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    int matrix_rows = 0;
    int matrix_columns = 0;
    file_input >> matrix_rows >> matrix_columns;
    if (matrix_rows * matrix_columns == 0) {
        matrix_rows = matrix_columns = 0;
    }
    Matrix matrix(matrix_rows, matrix_columns);
    int current_element = 0;
    if (matrix_rows == 0) {
        registers[register_index] = matrix;
        return;
    }
    for (int i = 0; i < matrix_rows; i++) {
        for (int j = 0; j < matrix_columns; j++) {
            file_input >> current_element;
            matrix.set_matrix_element(i, j, current_element);
        }
    }
    registers[register_index] = std::move(matrix);
}

void Interpreter::print(const int register_index) noexcept {
    const Matrix matrix = registers[register_index];
    const int matrix_rows = registers[register_index].rows();
    const int matrix_columns = registers[register_index].columns();
    for (int i = 0; i < matrix_rows; i++) {
        for (int j = 0; j < matrix_columns; j++) {
            std::cout << matrix.get_matrix_element(i, j);
            if (j != matrix_columns - 1) {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
}

void Interpreter::elem(
    const int register_index,
    const int row,
    const int column
) {
    std::cout << registers[register_index].get_matrix_element(row, column)
              << "\n";
}

void Interpreter::add(
    const int lhs_register_index,
    const int rhs_register_index
) {
    registers[lhs_register_index] += registers[rhs_register_index];
}

void Interpreter::mul(
    const int lhs_register_index,
    const int rhs_register_index
) {
    registers[lhs_register_index] *= registers[rhs_register_index];
}

}  // namespace matrix_interpreter

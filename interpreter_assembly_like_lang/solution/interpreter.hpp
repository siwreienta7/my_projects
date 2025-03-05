#ifndef INTERPRETER_HPP_
#define INTERPRETER_HPP_

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "matrix.hpp"

namespace matrix_interpreter {

struct invalid_command_format_error : std::runtime_error {
    invalid_command_format_error()
        : std::runtime_error("Invalid command format") {
    }
};

inline const int REGISTERS_QUANTITY = 10;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
enum class Command {
    LOAD,
    EXIT,
    NOTHING_TO_DO,
    UNKNOWN,
    PRINT,
    ELEM,
    ADD,
    MUL,
};

inline const std::unordered_map<std::string, Command> INPUT_TO_COMMANDS = {
    {"load", Command::LOAD}, {"exit", Command::EXIT}, {"print", Command::PRINT},
    {"elem", Command::ELEM}, {"add", Command::ADD},   {"mul", Command::MUL}};

std::pair<Command, std::vector<std::string>> parse_commands(
    const std::string &command
) noexcept;

struct Interpreter {
    void load(int register_index, const std::string &file);
    void print(int register_index) noexcept;
    void elem(int register_index, int row, int column);
    void add(int lhs_register_index, int rhs_register_index);
    void mul(int lhs_register_index, int rhs_register_index);

    Interpreter()
        : registers(std::vector<Matrix>(REGISTERS_QUANTITY, Matrix(0, 0))) {
    }

private:
    std::vector<Matrix> registers;
};

}  // namespace matrix_interpreter

#endif

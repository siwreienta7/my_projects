#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "interpreter.hpp"
#include "matrix.hpp"
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

namespace matrix_interpreter {
void check_invalid_format(const int size_of_tokens, const int declared_size) {
    if (size_of_tokens != declared_size) {
        throw std::runtime_error("Invalid command format");
    }
}

int parse_register(const std::string &argument) {
    if (argument.size() != 2 || argument[0] != '$' ||
        std::isdigit(argument[1]) == 0) {
        throw std::runtime_error("'" + argument + "' is not a register");
    }
    return argument[1] - '0';
}

bool is_correct_number(const std::string &argument) noexcept {
    const int length = static_cast<int>(argument.size());
    int count_zero = 0;
    for (int i = 0; i < length; i++) {
        if (isdigit(argument[i]) == 0 || i - count_zero > 6) {
            return false;
        }
        if (argument[i] == '0') {
            count_zero++;
        }
    }
    const int argument_int = std::stoi(argument);
    return (argument_int <= 1000000);
}
}  // namespace matrix_interpreter

int main() {
#ifdef _MSC_VER
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif
    std::string command_from_user;
    matrix_interpreter::Interpreter interpreter;
    while (true) {
        if (!std::getline(std::cin, command_from_user)) {
            return 0;
        }
        auto [command_name, tokens] =
            matrix_interpreter::parse_commands(command_from_user);
        try {
            switch (command_name) {
                case matrix_interpreter::Command::LOAD:
                    matrix_interpreter::check_invalid_format(
                        static_cast<int>(tokens.size()), 3
                    );
                    interpreter.load(
                        matrix_interpreter::parse_register(tokens[1]), tokens[2]
                    );
                    break;
                case matrix_interpreter::Command::PRINT:
                    matrix_interpreter::check_invalid_format(
                        static_cast<int>(tokens.size()), 2
                    );
                    interpreter.print(
                        matrix_interpreter::parse_register(tokens[1])
                    );
                    break;
                case matrix_interpreter::Command::ELEM:
                    matrix_interpreter::check_invalid_format(
                        static_cast<int>(tokens.size()), 4
                    );
                    if (!matrix_interpreter::is_correct_number(tokens[2]) ||
                        !matrix_interpreter::is_correct_number(tokens[3])) {
                        throw matrix_interpreter::invalid_command_format_error(
                        );
                    }
                    interpreter.elem(
                        matrix_interpreter::parse_register(tokens[1]),
                        std::stoi(tokens[2]), std::stoi(tokens[3])
                    );
                    break;
                case matrix_interpreter::Command::ADD:
                    matrix_interpreter::check_invalid_format(
                        static_cast<int>(tokens.size()), 3
                    );
                    interpreter.add(
                        matrix_interpreter::parse_register(tokens[1]),
                        matrix_interpreter::parse_register(tokens[2])
                    );
                    break;
                case matrix_interpreter::Command::MUL:
                    matrix_interpreter::check_invalid_format(
                        static_cast<int>(tokens.size()), 3
                    );
                    interpreter.mul(
                        matrix_interpreter::parse_register(tokens[1]),
                        matrix_interpreter::parse_register(tokens[2])
                    );
                    break;

                case matrix_interpreter::Command::EXIT:
                    matrix_interpreter::check_invalid_format(
                        static_cast<int>(tokens.size()), 1
                    );
                    return 0;
                case matrix_interpreter::Command::NOTHING_TO_DO:
                    throw matrix_interpreter::invalid_command_format_error();
                    break;
                case matrix_interpreter::Command::UNKNOWN:
                    throw std::runtime_error(
                        "Unknown command: '" + tokens[0] + "'"
                    );
                    break;
            }
        } catch (std::bad_alloc &) {
            std::cout << "Unable to allocate memory\n";
        } catch (std::out_of_range &) {
            std::cout << "Requested element is out of bounds\n";
        } catch (std::ios_base::failure &) {
            std::cout << "Invalid file format\n";
        } catch (std::exception &e) {
            std::cout << e.what() << "\n";
        }
    }
    return 0;
}

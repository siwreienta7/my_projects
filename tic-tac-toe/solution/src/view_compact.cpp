#include <boost/algorithm/string.hpp>
#include <boost/config.hpp>
#include <boost/dll/alias.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include "game.hpp"
#include "view.hpp"

namespace tictactoe {

namespace {
void use_zero() {
#ifdef VIEW_COMPACT_USE_ZERO
    std::cout << '0';
#else
    std::cout << 'O';
#endif
}

bool is_correct_number(const std::string &argument) {
    const int length = static_cast<int>(argument.size());
    int i = (argument[0] == '-') ? 1 : 0;
    for (; i < length; i++) {
        if (isdigit(argument[i]) == 0) {
            return false;
        }
    }
    return true;
}
}  // namespace

struct ViewCompact final : public View {
    void field_output(Game &game) override {
        std::vector<std::vector<OptionalPlayer>> field = game.get_field();
        for (int i = 0; i < SIZE_OF_FIELD; i++) {
            for (int j = 0; j < SIZE_OF_FIELD; j++) {
                switch (field[i][j]) {
                    case OptionalPlayer::NOTHING:
                        std::cout << ".";
                        break;
                    case OptionalPlayer::CROSS:
                        std::cout << "X";
                        break;
                    case OptionalPlayer::ZERO:
                        use_zero();
                        break;
                }
            }
            std::cout << "\n";
        }
    }

    std::optional<std::tuple<int, int, ResultPlay>> parse_input(
        std::string &input
    ) override {
        boost::algorithm::trim(input);
        if (input.substr(0, 5) == "view-") {
            return std::make_tuple(0, 0, ResultPlay::CHANGE_VIEW);
        }
        std::stringstream parse_strim(input);
        std::vector<std::string> result_of_stream;
        std::string current_arg;
        while (parse_strim >> current_arg) {
            result_of_stream.push_back(current_arg);
        }
        if (result_of_stream.size() != 2 ||
            !is_correct_number(result_of_stream[0]) ||
            !is_correct_number(result_of_stream[1])) {
            return std::nullopt;
        }
        const int first_arg = std::stoi(result_of_stream[0]);
        const int second_arg = std::stoi(result_of_stream[1]);
        if (first_arg == -1 && second_arg == -1) {
            return std::make_tuple(-1, -1, ResultPlay::EXIT);
        }
        return std::make_tuple(first_arg, second_arg, ResultPlay::OK);
    }
};
}  // namespace tictactoe

extern "C" BOOST_SYMBOL_EXPORT tictactoe::ViewCompact const plugin;
tictactoe::ViewCompact const plugin;

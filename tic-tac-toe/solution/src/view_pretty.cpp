#include <boost/algorithm/string.hpp>
#include <boost/config.hpp>
#include <boost/dll/alias.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>
#include "game.hpp"
#include "view.hpp"

namespace tictactoe {

namespace {
void for_cross(const int sect_i, const int sect_j) {
    if (sect_i + sect_j == 4) {
        std::cout << "/";
    } else {
        std::cout << "\\";
    }
}
}  // namespace

struct ViewPretty final : public View {
private:
    const int PASS_FOR_I = 3 * SIZE_OF_FIELD + 1;
    const int PASS_FOR_J = 5 * SIZE_OF_FIELD + 1;

    void field_output(Game &game) override {
        std::vector<std::vector<OptionalPlayer>> field = game.get_field();
        for (int i = 0; i < PASS_FOR_I; i++) {
            if (i % 3 == 1) {
                std::cout << SIZE_OF_FIELD - 1 - (i / 3) << " ";
            } else {
                std::cout << "  ";
            }
            for (int j = 0; j < PASS_FOR_J; j++) {
                if (i % 3 == 0 && j % 5 == 0) {
                    std::cout << "+";
                } else if (i % 3 != 0 && j % 5 == 0) {
                    std::cout << "|";
                } else if (i % 3 == 0 && j % 5 != 0) {
                    std::cout << "-";
                } else if (j % 5 == 1 || j % 5 == 4) {
                    std::cout << " ";
                } else {
                    const int current_y = i / 3;
                    const int current_x = j / 5;
                    switch (field[current_y][current_x]) {
                        case OptionalPlayer::CROSS:
                            for_cross(i % 3, j % 5);
                            break;
                        case OptionalPlayer::ZERO:
                            std::cout << "@";
                            break;
                        case OptionalPlayer::NOTHING:
                            std::cout << " ";
                            break;
                    }
                }
            }
            std::cout << "\n";
        }
        for (int i = 0; i < PASS_FOR_J + 2; i++) {
            if (i % 5 == 4) {
                std::cout << std::string(1, static_cast<char>('a' + (i / 5)));
            } else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }

    std::optional<std::tuple<int, int, ResultPlay>> parse_input(
        std::string &input
    ) override {
        if (input.substr(0, 5) == "view-") {
            return std::make_tuple(0, 0, ResultPlay::CHANGE_VIEW);
        }
        if (input.size() != 2) {
            return std::nullopt;
        }
        if (input[0] >= 'a' && input[0] <= 'j' && input[1] >= '0' &&
            input[1] <= '9') {
            const int first_arg = SIZE_OF_FIELD - 1 - (input[1] - '0');
            const int second_arg = input[0] - 'a';
            return std::make_tuple(first_arg, second_arg, ResultPlay::OK);
        }
        return std::nullopt;
    }
};

}  // namespace tictactoe

extern "C" BOOST_SYMBOL_EXPORT tictactoe::ViewPretty const plugin;
tictactoe::ViewPretty const plugin;

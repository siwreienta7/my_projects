#include "game.hpp"
#include <boost/algorithm/cxx11/any_of.hpp>
#include <cmath>

namespace tictactoe {
Game::Game() {
    game_field.resize(SIZE_OF_FIELD);
    std::fill(
        game_field.begin(), game_field.end(),
        std::vector<OptionalPlayer>(SIZE_OF_FIELD, OptionalPlayer::NOTHING)
    );
}

std::optional<ResultPlay> Game::make_move(const int x, const int y) {
    if (!check_correctness_move(x, y)) {
        return std::nullopt;
    }
    game_field[x][y] = current_player;
    if (check_winner(x, y)) {
        return (current_player == OptionalPlayer::CROSS) ? ResultPlay::WIN_X
                                                         : ResultPlay::WIN_O;
    }
    current_player = (current_player == OptionalPlayer::CROSS)
                         ? OptionalPlayer::ZERO
                         : OptionalPlayer::CROSS;
    free_cells--;
    if (free_cells == 0) {
        return ResultPlay::DRAW;
    }
    return ResultPlay::CONTINUE;
}

bool Game::check_winner(const int x, const int y) {
    int ordinate = 0;
    int abscissa = 0;
    int diagonal_1 = 0;
    int diagonal_2 = 0;
    for (int i = 0; i < SIZE_OF_FIELD; i++) {
        const int x_for_diag1 = i - (x - y);
        const int x_for_diag2 = (x + y) - i;
        if (x_for_diag1 >= 0 && x_for_diag1 < SIZE_OF_FIELD) {
            diagonal_1 = (game_field[i][x_for_diag1] == current_player)
                             ? diagonal_1 + 1
                             : 0;
        }
        if (x_for_diag2 >= 0 && x_for_diag2 < SIZE_OF_FIELD) {
            diagonal_2 = (game_field[i][x_for_diag2] == current_player)
                             ? diagonal_2 + 1
                             : 0;
        }
        abscissa = (game_field[x][i] == current_player) ? abscissa + 1 : 0;
        ordinate = (game_field[i][y] == current_player) ? ordinate + 1 : 0;
        if (ordinate == CELLS_TO_WIN || abscissa == CELLS_TO_WIN ||
            diagonal_1 == CELLS_TO_WIN || diagonal_2 == CELLS_TO_WIN) {
            return true;
        }
    }
    return false;
}

bool Game::check_correctness_move(const int x, const int y) {
    if (x >= SIZE_OF_FIELD || y >= SIZE_OF_FIELD || x < 0 || y < 0) {
        return false;
    }
    if (game_field[x][y] != OptionalPlayer::NOTHING) {
        return false;
    }
    return true;
}

}  // namespace tictactoe

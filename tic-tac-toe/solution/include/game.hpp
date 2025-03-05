#ifndef GAME_HPP_
#define GAME_HPP_

#include <optional>
#include <vector>
#include "game_fwd.hpp"

namespace tictactoe {
struct Game {
private:
    OptionalPlayer current_player = OptionalPlayer::ZERO;
    std::vector<std::vector<OptionalPlayer>> game_field;
    int free_cells = 100;

public:
    explicit Game();
    std::optional<ResultPlay> make_move(int, int);
    bool check_winner(int, int);
    bool check_correctness_move(int, int);

    std::vector<std::vector<OptionalPlayer>> get_field() {
        return game_field;
    }

    OptionalPlayer get_player() {
        return current_player;
    }
};
}  // namespace tictactoe

#endif

#ifndef GAME_FWD_HPP_
#define GAME_FWD_HPP_

namespace tictactoe {
struct Game;
enum class OptionalPlayer {
    NOTHING,
    CROSS,
    ZERO,
};
enum class ResultPlay { WIN_O, WIN_X, DRAW, EXIT, CONTINUE, CHANGE_VIEW, OK };

const int SIZE_OF_FIELD = 10;
const int CELLS_TO_WIN = 5;

}  // namespace tictactoe

#endif

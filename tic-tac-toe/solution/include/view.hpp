#ifndef VIEW_HPP_
#define VIEW_HPP_

#include <boost/config.hpp>
#include <optional>
#include <string>
#include "game_fwd.hpp"

namespace tictactoe {
struct BOOST_SYMBOL_VISIBLE
    View {  // NOLINT(cppcoreguidelines-special-member-functions)
    virtual ~View() = default;
    virtual void field_output(Game &) = 0;
    virtual std::optional<std::tuple<int, int, ResultPlay>>
    parse_input(std::string &) = 0;
};
}  // namespace tictactoe
#endif
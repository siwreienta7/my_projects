#ifdef _MSC_VER
#include <crtdbg.h>
#endif
#include <boost/dll/import.hpp>
#include <boost/version.hpp>
#include <game.hpp>
#include <game_fwd.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <view.hpp>

#if BOOST_VERSION >= 107600
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define boost_dll_import_symbol ::boost::dll::import_symbol
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define boost_dll_import_symbol ::boost::dll::import
#endif

void invitation(tictactoe::Game &game) {
    switch (game.get_player()) {
        case tictactoe::OptionalPlayer::CROSS:
            std::cout << "X move: ";
            break;
        case tictactoe::OptionalPlayer::ZERO:
            std::cout << "O move: ";
            break;
        default:
            break;
    }
}

const std::unordered_map<tictactoe::ResultPlay, std::string> TEXT_TO_END_GAME{
    {tictactoe::ResultPlay::WIN_O, "O wins!\n"},
    {tictactoe::ResultPlay::WIN_X, "X wins!\n"},
    {tictactoe::ResultPlay::DRAW, "Draw.\n"}};

int main(int argc, char *argv[]) {
#ifdef _MSC_VER
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif
    if (argc != 2) {
        std::cout << "Expected exactly one argument: name of the view file to "
                     "load, found "
                  << (argc - 1) << "\n";
        return 1;
    }
    std::string request;

    tictactoe::Game game;
    boost::shared_ptr<tictactoe::View> view =
        boost_dll_import_symbol<tictactoe::View>(
            argv[1],  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            "plugin", boost::dll::load_mode::append_decorations
        );
    std::cout << "\n";

    view->field_output(game);
    invitation(game);
    while (true) {
        if (!std::getline(std::cin, request)) {
            return 0;
        }
        auto parsed_input = view->parse_input(request);
        if (!parsed_input.has_value()) {
            std::cout << "Bad move!\n";
            invitation(game);
            continue;
        }
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        auto [first_arg, second_arg, stat] = *parsed_input;
        if (stat == tictactoe::ResultPlay::EXIT) {
            return 0;
        }
        if (stat == tictactoe::ResultPlay::CHANGE_VIEW) {
            view.reset();
            view = boost_dll_import_symbol<tictactoe::View>(
                request, "plugin", boost::dll::load_mode::append_decorations
            );
            view->field_output(game);
            invitation(game);
            continue;
        }
        auto parsed_move = game.make_move(first_arg, second_arg);
        if (!parsed_move.has_value()) {
            std::cout << "Bad move!\n";
            invitation(game);
            continue;
        }
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        auto result_of_move = *parsed_move;
        switch (result_of_move) {
            case tictactoe::ResultPlay::WIN_O:
            case tictactoe::ResultPlay::WIN_X:
            case tictactoe::ResultPlay::DRAW:
                std::cout << "\n";
                view->field_output(game);
                std::cout << TEXT_TO_END_GAME.at(result_of_move);
                return 0;
            case tictactoe::ResultPlay::CONTINUE:
                std::cout << "\n";

                view->field_output(game);
                invitation(game);
                break;
            case tictactoe::ResultPlay::EXIT:
            default:
                return 0;
        }
    }
}

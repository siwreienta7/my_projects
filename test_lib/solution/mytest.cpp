#include "mytest.hpp"
#include <optional>
#include <string>
#include "mytest_internal.hpp"

namespace mytest {
// cppcheck-suppress unusedFunction
void my_test_check_message(
    const std::string &expr,
    const std::string &file,
    const int line,
    std::optional<std::string> msg
) {
    test_case_controller.test_failed();
    std::cerr << "CHECK(" << expr << ") at " << file << ":" << line
              << " failed!\n";
    if (msg.has_value()) {
        std::cerr << "    message: " << msg.value() << "\n";
    }

    for (auto &subcase_name : subcase_controller.get_current_branch()) {
        std::cerr << "    in subcase " << subcase_name << "\n";
    }
}

// cppcheck-suppress unusedFunction
void my_test_case_register(
    std::function<void()> func,
    const std::string &name
) {
    test_case_controller.add_test(TestCase{name, std::move(func)});
}

bool run_tests() {
    test_case_controller.sort_cases();
    std::size_t current_passed_tests = 0;
    for (const auto &test_case : test_case_controller.get_test_cases()) {
        std::cerr << "Running \"" << test_case.name << "\"...\n";
        const int current_tests_failure =
            test_case_controller.get_failed_tests();
        bool has_subcases = true;
        while (has_subcases) {
            const int initial_failed = test_case_controller.get_failed_tests();
            test_case.func_with_tests();
            subcase_controller.reset_progress();

            if (initial_failed < test_case_controller.get_failed_tests()) {
                break;
            }

            has_subcases = (subcase_controller.get_max_level() != 0);

            if (has_subcases) {
                std::cerr << "...another subcase...\n";
            }
        }
        if (current_tests_failure == test_case_controller.get_failed_tests()) {
            current_passed_tests++;
        }
    }
    std::cerr << "===== Tests passed: " << current_passed_tests << "/"
              << test_case_controller.get_test_cases().size() << " =====\n";
    return (
        test_case_controller.get_test_cases().size() == current_passed_tests
    );
}

}  // namespace mytest
#ifndef MYTEST_HPP
#define MYTEST_HPP

#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace mytest {

void my_test_check_message(
    const std::string &expr,
    const std::string &file,
    int line,
    std::optional<std::string> msg
);
void my_test_case_register(std::function<void()> func, const std::string &name);
bool run_tests();

struct TestCase {
    std::string name;
    std::function<void()> func_with_tests;
};

struct TestCaseController {
    void test_failed() {
        count_of_failed_tests++;
    }

    [[nodiscard]] int get_failed_tests() const {
        return count_of_failed_tests;
    }

    void add_test(const TestCase &_my_test_case) {
        test_cases.push_back(_my_test_case);
    }

    std::vector<TestCase> &get_test_cases() {
        return test_cases;
    }

    void sort_cases() {
        std::sort(
            test_cases.begin(), test_cases.end(),
            [](const TestCase &a, const TestCase &b) { return a.name < b.name; }
        );
    }

private:
    int count_of_failed_tests{0};
    std::vector<TestCase> test_cases;
};

struct SubcaseController {
    void reset_progress() {
        executed_levels.clear();
        current_branch.clear();
        current_path.clear();
    }

    std::vector<std::string> get_current_path() {
        return current_path;
    }

    void update_current_path(const std::string &_subcase) {
        current_path.push_back(_subcase);
    }

    std::deque<std::string> get_subcase_tree_node(int _depth) {
        return subcase_tree[_depth];
    }

    void update_subcase_tree_node(const std::string &_value) {
        subcase_tree[current_subcase_depth].push_back(_value);
    }

    void pop_subcase_tree_node() {
        subcase_tree[current_subcase_depth].pop_front();
    }

    [[nodiscard]] int get_current_level() const {
        return current_subcase_depth;
    }

    void update_executed_levels_and_subcase_tree() {
        while (executed_levels.size() <=
               static_cast<std::size_t>(max_subcase_depth)) {
            executed_levels.push_back(false);
        }
        while (subcase_tree.size() <=
               static_cast<std::size_t>(max_subcase_depth)) {
            subcase_tree.emplace_back();
        }
    }

    void change_current_level(int _addend) {
        current_subcase_depth += _addend;
        max_subcase_depth = std::max(max_subcase_depth, current_subcase_depth);
        update_executed_levels_and_subcase_tree();
    }

    void add_to_max_level(int _addend) {
        max_subcase_depth += _addend;
        update_executed_levels_and_subcase_tree();
    }

    [[nodiscard]] int get_max_level() const {
        return max_subcase_depth;
    }

    std::set<std::string> get_existing_subcases() {
        return existing_subcases;
    }

    void update_existing_subcases(const std::string &_subcase) {
        existing_subcases.insert(_subcase);
    }

    bool get_full_executed_subcases(int _subcase) {
        return executed_levels[_subcase];
    }

    void set_subcase_as_full_executed(int _subcase) {
        executed_levels[_subcase] = true;
    }

    std::vector<std::string> get_current_branch() {
        return current_branch;
    }

    void update_current_branch(const std::string &_subcase_name) {
        current_branch.push_back(_subcase_name);
    }

    bool is_leave(const std::string &_subcase) {
        return _subcase == subcase_tree[current_subcase_depth].front() &&
               !executed_levels[current_subcase_depth];
    }

private:
    int current_subcase_depth = 0;
    int max_subcase_depth = 0;
    std::vector<std::string> current_branch;
    std::vector<std::deque<std::string>> subcase_tree;
    std::set<std::string> existing_subcases;
    std::vector<std::string> current_path;
    std::vector<bool> executed_levels;
};

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
inline SubcaseController subcase_controller;
inline TestCaseController test_case_controller;

// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

struct Subcase {
    Subcase(const Subcase &) = delete;
    Subcase(Subcase &&) = delete;
    Subcase &operator=(const Subcase &) = delete;
    Subcase &operator=(Subcase &&) = delete;

    explicit Subcase(std::string _name) : name(std::move(_name)) {
        if (is_executed) {
            return;
        }

        for (auto &path : subcase_controller.get_current_path()) {
            if (path == name + std::to_string(counter)) {
                // cppcheck-suppress useStlAlgorithm
                counter++;
            }
        }
        name_for_controller = name + std::to_string(counter);
        subcase_controller.update_current_path(name_for_controller);

        subcase_controller.change_current_level(1);

        if (!subcase_controller.get_existing_subcases().contains(
                name_for_controller
            )) {
            subcase_controller.update_subcase_tree_node(name_for_controller);
            subcase_controller.update_existing_subcases(name_for_controller);
        }

        if (subcase_controller.is_leave(name_for_controller)) {
            subcase_controller.update_current_branch(name);
        }
    }

    ~Subcase() {
        if (is_executed) {
            return;
        }

        if (subcase_controller.get_current_level() ==
                subcase_controller.get_max_level() &&
            subcase_controller.is_leave(name_for_controller)) {
            subcase_controller.pop_subcase_tree_node();
            is_executed = true;
            if (subcase_controller
                    .get_subcase_tree_node(subcase_controller.get_max_level())
                    .empty()) {
                subcase_controller.add_to_max_level(-1);
            }
            subcase_controller.set_subcase_as_full_executed(
                subcase_controller.get_current_level()
            );
        }

        subcase_controller.change_current_level(-1);
    }

    operator bool() const {
        return subcase_controller.is_leave(name_for_controller);
    }

private:
    std::string name;
    int counter = 0;
    std::string name_for_controller;
    bool is_executed{false};
};

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define MYTEST_INTERNAL_CHECK(expr, msg) \
    ((void                               \
    )((expr) ||                          \
      (::mytest::my_test_check_message(#expr, __FILE__, __LINE__, (msg)), 0)))

#define CHECK(expr) MYTEST_INTERNAL_CHECK(expr, ::std::nullopt)
#define CHECK_MESSAGE(expr, msg) MYTEST_INTERNAL_CHECK(expr, msg)

#define MYTEST_INTERNAL_CREATE_NAME(first_part, second_part) \
    first_part##second_part
#define MYTEST_INTERNAL_GET_NAME(first_part, second_part) \
    MYTEST_INTERNAL_CREATE_NAME(first_part, second_part)
#define MYTEST_INTERNAL_UNIQUE_NAME(name) \
    MYTEST_INTERNAL_GET_NAME(name, __LINE__)

#define MYTEST_INTERNAL_REGISTRATION(func) \
    static int MYTEST_INTERNAL_UNIQUE_NAME(mytest_var) = ((func), 0);

#define TEST_CASE_REGISTER(func, name) \
    MYTEST_INTERNAL_REGISTRATION(::mytest::my_test_case_register(func, name))
#define TEST_CASE(name)                                           \
    static void MYTEST_INTERNAL_UNIQUE_NAME(mytest_func)();       \
    MYTEST_INTERNAL_REGISTRATION(::mytest::my_test_case_register( \
        MYTEST_INTERNAL_UNIQUE_NAME(mytest_func), name            \
    ));                                                           \
    static void MYTEST_INTERNAL_UNIQUE_NAME(mytest_func)()

#define SUBCASE(name)                                                         \
    if (const ::mytest::Subcase & MYTEST_INTERNAL_UNIQUE_NAME(MYTEST_SUBCASE_ \
                                  ) = ::mytest::Subcase(name))
// NOLINTEND(cppcoreguidelines-macro-usage)
}  // namespace mytest
#endif

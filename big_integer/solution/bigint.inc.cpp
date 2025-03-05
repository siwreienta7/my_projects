#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
// NOLINTNEXTLINE(readability-redundant-declaration)
extern const int BASE;
// NOLINTNEXTLINE(cppcoreguidelines-interfaces-global-init)
static const int LOG_10 = static_cast<int>(log10(BASE));

namespace lab_bigint {

class bigint {
    std::vector<unsigned int> digits;

public:
    bigint(const std::string &s = "0") {
        string_to_vector(s);
    }

    // cppcheck-suppress noExplicitConstructor
    [[maybe_unused]] bigint(unsigned int n) {
        if (n == 0) {
            digits.push_back(0);
        } else {
            while (n > 0) {
                digits.push_back(n % BASE);
                n /= BASE;
            }
            trim();
        }
    }

    explicit operator unsigned int() const {
        unsigned int result = 0;
        unsigned int current_base = 1;
        for (auto digit : digits) {
            result += digit * current_base;
            current_base *= BASE;
        }
        return result;
    }

    friend bool operator==(const bigint &lhs, const bigint &rhs) {
        return lhs.digits == rhs.digits;
    }

    friend bool operator<(const bigint &lhs, const bigint &rhs) {
        if (lhs.digits.size() != rhs.digits.size()) {
            return lhs.digits.size() < rhs.digits.size();
        }
        const int length = static_cast<int>(lhs.digits.size());
        for (int i = length - 1; i >= 0; i--) {
            if (lhs.digits[i] != rhs.digits[i]) {
                return (lhs.digits[i] < rhs.digits[i]);
            }
        }
        return false;
    }

    friend bool operator>(const bigint &lhs, const bigint &rhs) {
        return rhs < lhs;
    }

    friend bool operator>=(const bigint &lhs, const bigint &rhs) {
        return !(lhs < rhs);
    }

    friend bool operator<=(const bigint &lhs, const bigint &rhs) {
        return !(lhs > rhs);
    }

    bigint &operator+=(const bigint &rhs) {
        int left_length = static_cast<int>(digits.size());
        const int right_length = static_cast<int>(rhs.digits.size());
        unsigned int add_term = 0;
        if (left_length < right_length) {
            for (int i = 0; i < right_length - left_length; i++) {
                digits.push_back(0);
            }
            left_length = right_length;
        }
        for (int i = 0; i < left_length; i++) {
            unsigned int current_sum = digits[i] + add_term;
            if (i < right_length) {
                current_sum += rhs.digits[i];
            }
            add_term = current_sum / BASE;
            digits[i] = current_sum % BASE;
        }
        if (add_term != 0) {
            digits.push_back(add_term);
        }
        return *this;
    }

    friend bigint operator+(const bigint &lhs, const bigint &rhs) {
        return bigint(lhs) += rhs;
    }

    bigint &operator-=(const bigint &rhs) {
        const int left_length = static_cast<int>(digits.size());
        const int right_length = static_cast<int>(rhs.digits.size());
        int del_term = 0;
        for (int i = 0; i < left_length; i++) {
            int current_difference = static_cast<int>(digits[i]) - del_term;
            if (i < right_length) {
                current_difference -= static_cast<int>(rhs.digits[i]);
            }
            digits[i] =
                static_cast<unsigned int>((current_difference + BASE) % BASE);
            del_term =
                (static_cast<int>(digits[i]) - current_difference) / BASE;
        }
        trim();
        return *this;
    }

    friend bigint operator-(const bigint &lhs, const bigint &rhs) {
        return bigint(lhs) -= rhs;
    }

    bigint &operator++() {
        return *this += 1;
    }

    bigint operator++(int) {
        bigint result = *this;
        ++(*this);
        return result;
    }

    bigint &operator--() {
        return *this -= 1;
    }

    bigint operator--(int) {
        bigint result = *this;
        --(*this);
        return result;
    }

    [[nodiscard]] std::string to_string() const {
        std::string s;
        std::string current_number_to_string;
        const int length = static_cast<int>(digits.size());
        for (int i = length - 1; i >= 0; i--) {
            current_number_to_string = std::to_string(digits[i]);
            s += std::string(
                     std::max(
                         static_cast<int>(
                             LOG_10 - current_number_to_string.size()
                         ),
                         0
                     ),
                     '0'
                 ) +
                 current_number_to_string;
        }
        while (s.size() > 1 && s[0] == '0') {
            s.erase(0, 1);
        }
        return s;
    }

    void trim() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
    }

    friend std::ostream &operator<<(std::ostream &os, const bigint &a) {
        return os << a.to_string();
    }

    friend std::istream &operator>>(std::istream &is, bigint &a) {
        std::string s;
        is >> s;
        a.string_to_vector(s);
        return is;
    }

    void string_to_vector(const std::string &s) {
        digits.clear();
        if (s == "0") {
            digits.push_back(0);
            return;
        }
        const int length = static_cast<int>(s.size());
        for (int i = length - 1; i >= 0; i -= LOG_10) {
            if (i - LOG_10 + 1 < 0) {
                digits.push_back(std::stoi(s.substr(0, i + 1)));
            } else {
                digits.push_back(std::stoi(s.substr(i - LOG_10 + 1, LOG_10)));
            }
        }
        trim();
    }
};

}  // namespace lab_bigint

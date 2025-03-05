#ifndef BANK_HPP_
#define BANK_HPP_

#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace bank {
struct transfer_error : std::exception {
    explicit transfer_error(std::string message)
        : m_message(std::move(message)) {
    }

private:
    std::string m_message;

    [[nodiscard]] const char *what() const noexcept override {
        return m_message.c_str();
    }
};

struct not_enough_funds_error : transfer_error {
    not_enough_funds_error(int balance_of_sender, int transfer_amount)
        : transfer_error(
              "Not enough funds: " + std::to_string(balance_of_sender) +
              " XTS available, " + std::to_string(transfer_amount) +
              " XTS requested"
          ) {
    }
};

struct another_transfer_error : transfer_error {
    explicit another_transfer_error(std::string message)
        : transfer_error(std::move(message)) {
    }
};

struct user;
struct user_transactions_iterator;

struct transaction {
    transaction(const user *_counterparty, int _delta, std::string _comment)
        : counterparty(_counterparty),
          balance_delta_xts(_delta),
          comment(std::move(_comment)) {
    }

    // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
    const user *const counterparty;
    const int balance_delta_xts;
    const std::string comment;
    // NOLINTEND(misc-non-private-member-variables-in-classes)
};

struct user {
    explicit user(const std::string &);

    std::string name() const noexcept {
        return m_name;
    }

    int balance_xts() const {
        return m_balance;
    }

    user_transactions_iterator
    snapshot_transactions(const std::function<
                          void(const std::vector<transaction> &, const int)> &)
        const;
    void transfer(user &, int, std::string);

    user_transactions_iterator monitor() const;

private:
    std::string m_name;
    int m_balance;
    std::vector<transaction> m_transactions;
    std::size_t m_count_of_transactions;
    mutable std::mutex m_mutex;
    mutable std::condition_variable m_cond_var;
    friend struct user_transactions_iterator;
};

struct ledger {
    user &get_or_create_user(const std::string &);

private:
    std::unordered_map<std::string, std::unique_ptr<user>> m_users;
    std::mutex m_mutex;
};

struct user_transactions_iterator {
    user_transactions_iterator(const user *_user, std::size_t _count_of_trans)
        : m_user(_user), m_count_of_trans(_count_of_trans) {
    }

    transaction wait_next_transaction() {
        std::unique_lock<std::mutex> l(m_user->m_mutex);
        m_user->m_cond_var.wait(l, [this]() {
            return m_user->m_count_of_transactions > m_count_of_trans;
        });
        return m_user->m_transactions[m_count_of_trans++];
    }

private:
    const user *m_user;
    std::size_t m_count_of_trans;
};
}  // namespace bank

#endif
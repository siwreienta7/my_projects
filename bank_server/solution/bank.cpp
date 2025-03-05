#include "bank.hpp"
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace bank {
user::user(const std::string &name)
    : m_name(name), m_balance(100), m_count_of_transactions(1) {
    m_transactions.emplace_back(nullptr, 100, "Initial deposit for " + name);
}

user_transactions_iterator user::snapshot_transactions(
    const std::function<void(const std::vector<transaction> &, const int)> &f
) const {
    const std::unique_lock<std::mutex> l(m_mutex);
    f(m_transactions, m_balance);
    return {this, m_transactions.size()};
}

user_transactions_iterator user::monitor() const {
    const std::unique_lock<std::mutex> l(m_mutex);
    return {this, m_count_of_transactions};
}

void user::transfer(user &counterparty, int amount_xts, std::string comment) {
    const std::scoped_lock l(m_mutex, counterparty.m_mutex);
    if (amount_xts > m_balance) {
        throw not_enough_funds_error(m_balance, amount_xts);
    }
    if (amount_xts < 0) {
        throw another_transfer_error("Do you want to request a transaction?)");
    }
    m_balance -= amount_xts;
    counterparty.m_balance += amount_xts;
    m_transactions.emplace_back(&counterparty, -amount_xts, comment);
    m_count_of_transactions++;
    counterparty.m_transactions.emplace_back(
        this, amount_xts, std::move(comment)
    );
    counterparty.m_count_of_transactions++;
    m_cond_var.notify_all();
    counterparty.m_cond_var.notify_all();
}

user &ledger::get_or_create_user(const std::string &name) {
    const std::unique_lock<std::mutex> l(m_mutex);
    if (m_users.contains(name)) {
        return *m_users[name];
    }
    m_users[name] = std::make_unique<user>(name);
    return *m_users[name];
}

}  // namespace bank

#ifdef _MSC_VER
#include <crtdbg.h>
#endif
#include <exception>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include "bank.hpp"
#include "boost/asio.hpp"

namespace bank {
enum class Command {
    BALANCE,
    TRANSACTIONS,
    MONITOR,
    TRANSFER,
    UNKNOWN,
};

const std::unordered_map<std::string, Command> string_to_cmd = {
    {"balance", Command::BALANCE},
    {"transactions", Command::TRANSACTIONS},
    {"monitor", Command::MONITOR},
    {"transfer", Command::TRANSFER},
};

struct server_bank {
    server_bank(
        unsigned short _port,
        std::string &_port_file,
        boost::asio::ip::tcp::acceptor &_acceptor
    )
        : m_acceptor(std::move(_acceptor)) {
        std::ofstream port_file_stream(_port_file);
        if (!port_file_stream) {
            throw std::runtime_error(
                "Unable to store port to file " + _port_file
            );
        }
        port_file_stream
            << ((_port != 0) ? _port : m_acceptor.local_endpoint().port());
        std::cout << "Listening at " << m_acceptor.local_endpoint() << "\n";
    }

    void run_server() {
        while (true) {
            boost::asio::ip::tcp::socket s = m_acceptor.accept();
            std::thread([this, socket_ = std::move(s)]() mutable {
                process_client_request(socket_);
            }).detach();
        }
    }

private:
    static void message_about_tr(
        const transaction &current_tr,
        boost::asio::ip::tcp::iostream &client
    ) {
        client << ((current_tr.counterparty == nullptr)
                       ? "-"
                       : current_tr.counterparty->name())
               << "\t" << current_tr.balance_delta_xts << "\t"
               << current_tr.comment << "\n";
    }

    static void last_transactions(
        const int n,
        boost::asio::ip::tcp::iostream &client,
        user &current_user
    ) {
        current_user.snapshot_transactions(
            [&](const std::vector<transaction> &current_trs,
                int current_balance) {
                client << "CPTY\tBAL\tCOMM\n";
                const int end = static_cast<int>(current_trs.size());
                const int begin = std::max(0, end - n);
                for (int i = begin; i < end; i++) {
                    message_about_tr(current_trs[i], client);
                }
                client << "===== BALANCE: " << current_balance
                       << " XTS =====\n";
            }
        );
    }

    void process_client_request(boost::asio::ip::tcp::socket &s) {
        std::cout << "Connected " << s.remote_endpoint() << " --> "
                  << s.local_endpoint() << "\n";
        boost::asio::ip::tcp::iostream client(std::move(s));
        std::string username;
        client << "What is your name?\n";
        client >> username;
        client << "Hi " << username << "\n";
        user *current_user = &m_ledger.get_or_create_user(username);
        std::string cmd;
        // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
        Command mode;
        while (client) {
            if (!(client >> cmd)) {
                break;
            }
            if (!string_to_cmd.contains(cmd)) {
                mode = Command::UNKNOWN;
            } else {
                mode = string_to_cmd.at(cmd);
            }
            switch (mode) {
                case Command::BALANCE:
                    client << current_user->balance_xts() << "\n";
                    break;
                case Command::TRANSACTIONS: {
                    int arg = 0;
                    client >> arg;
                    last_transactions(arg, client, *current_user);
                    break;
                }
                case Command::MONITOR: {
                    int arg = 0;
                    client >> arg;
                    last_transactions(arg, client, *current_user);
                    while (true) {
                        user_transactions_iterator it = current_user->monitor();
                        auto new_transaction = it.wait_next_transaction();
                        message_about_tr(new_transaction, client);
                    }
                    break;
                }
                case Command::TRANSFER: {
                    int amount = 0;
                    std::string comment;
                    std::string counterparty;
                    client >> counterparty >> amount;
                    std::getline(client, comment);
                    comment.erase(comment.begin());
                    try {
                        current_user->transfer(
                            m_ledger.get_or_create_user(counterparty), amount,
                            comment
                        );
                        client << "OK\n";
                    } catch (std::exception &e) {
                        client << e.what() << "\n";
                    }
                    break;
                }
                case Command::UNKNOWN:
                    client << "Unknown command: '" << cmd << "'\n";
                    break;
            }
        }
        std::cout << "Disconnected " << client.socket().remote_endpoint()
                  << " --> " << client.socket().local_endpoint() << "\n";
    }

    ledger m_ledger;
    boost::asio::ip::tcp::acceptor m_acceptor;
};
}  // namespace bank

int main(int argc, char *argv[]) {
#ifdef _MSC_VER
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif
    try {
        if (argc != 3) {
            throw std::runtime_error("You're banned\n");
        }
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        const std::string port_(argv[1]);
        const auto port_integer = static_cast<unsigned short>(std::stoi(port_));
        std::string port_file_(argv[2]);
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::acceptor acceptor(
            io_context, boost::asio::ip::tcp::endpoint(
                            boost::asio::ip::tcp::v4(), port_integer
                        )
        );
        bank::server_bank server(port_integer, port_file_, acceptor);
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        server.run_server();
    } catch (std::exception &e) {
        std::cerr << e.what() << "\n";
    }
}

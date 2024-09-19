#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <sstream>
#include "../structures/data_store.cpp"

namespace asio = boost::asio;
using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {

public:
    Session(tcp::socket socket, std::shared_ptr<DataStore> store)
            : socket_(std::move(socket)), store_(store) {
        std::cout << "new session created" << std::endl;
    }

    void start() {
        std::cout << "starting session" << std::endl;
        do_read();
    }

private:
    void do_read() {
        // create another shared ptr for this same instance, we do this because the async ops may complete after the original shared_ptr goes out of scope
        // by capturing additional shared_ptrs in the lambda, ensures session object remains alive til all pending async ops are complete
        auto self(shared_from_this());
        std::cout << "waiting for client data..." << std::endl;
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                [this, self](boost::system::error_code ec, std::size_t length) {
                                    if (!ec) {
                                        std::string message(data_, length);
                                        std::cout << "received: " << message << std::endl;
                                        std::string response = process_message(message);
                                        std::cout << "sending response: " << response << std::endl;
                                        do_write(response);
                                    } else {
                                        std::cerr << "read error: " << ec.message() << std::endl;
                                    }
                                });
    }

    void do_write(const std::string& message) {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(message + "\n"),
                                 [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                     if (!ec) {
                                         std::cout << "response sent, waiting for next command" << std::endl;
                                         do_read();
                                     } else {
                                         std::cerr << "write error: " << ec.message() << std::endl;
                                     }
                                 });
    }

    std::string process_message(const std::string& message) {
        std::istringstream iss(message);
        std::string command;
        iss >> command;

        try {
            if (command == "ZADD") {
                std::string key, member;
                double score;
                if (!(iss >> key >> score >> member)) {
                    return "error: ZADD requires a key, score, and member";
                }
                return store_->zadd(key, score, member) ? "1" : "0";
            } else if (command == "ZREM") {
                std::string key, member;
                if (!(iss >> key >> member)) {
                    return "error: ZREM requires a key and member";
                }
                return store_->zrem(key, member) ? "1" : "0";
            } else if (command == "ZSCORE") {
                std::string key, member;
                if (!(iss >> key >> member)) {
                    return "error: ZSCORE requires a key and member";
                }
                auto score = store_->zscore(key, member);
                return score ? std::to_string(*score) : "(nil)";
            } else if (command == "ZQUERY") {
                std::string key;
                double min_score, max_score;
                std::string min_member, max_member;
                int64_t offset, count;
                if (!(iss >> key >> min_score >> min_member >> max_score >> max_member >> offset >> count)) {
                    return "error: ZQUERY requires key, min_score, min_member, max_score, max_member, offset, and count";
                }
                auto result = store_->zquery(key, min_score, min_member, max_score, max_member, offset, count);
                std::ostringstream oss;
                oss << result.size() << "\n";
                for (const auto& pair : result) {
                    oss << pair.first << " " << pair.second << "\n";
                }
                return oss.str();
            } else {
                return "unknown command";
            }
        } catch (const std::exception& e) {
            std::cerr << "error processing command: " << e.what() << std::endl;
            return "error: " + std::string(e.what());
        }
    }

    tcp::socket socket_;
    std::shared_ptr<DataStore> store_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class Server {
public:
    Server(asio::io_context& io_context, short port)
            : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
              store_(std::make_shared<DataStore>()) {
        std::cout << "server created, starting to accept connections" << std::endl;
        do_accept();
    }

private:
    void do_accept() {
        std::cout << "waiting for a client to connect..." << std::endl;
        acceptor_.async_accept(
                [this](boost::system::error_code ec, tcp::socket socket) {
                    if (!ec) {
                        std::cout << "client connected from: " << socket.remote_endpoint() << std::endl;
                        std::make_shared<Session>(std::move(socket), store_)->start();
                    } else {
                        std::cerr << "accept error: " << ec.message() << std::endl;
                    }

                    do_accept();
                });
    }

    tcp::acceptor acceptor_;
    std::shared_ptr<DataStore> store_;
};

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "usage: server <port>\n";
            return 1;
        }

        asio::io_context io_context;
        Server server(io_context, std::atoi(argv[1]));
        std::cout << "server started on port " << argv[1] << std::endl;
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "exception in main: " << e.what() << "\n";
    }

    return 0;
}

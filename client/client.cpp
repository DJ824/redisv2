#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

class Client {
public:
    Client(boost::asio::io_context& io_context,
           const std::string& host, const std::string& port)
            : socket_(io_context) {
        try {
            tcp::resolver resolver(io_context);
            endpoints_ = resolver.resolve(host, port);
            boost::asio::connect(socket_, endpoints_);
            std::cout << "connected successfully.\n";
        } catch (std::exception& e) {
            std::cerr << "connection failed: " << e.what() << std::endl;
            throw;
        }
    }

    void run() {
        while (true) {
            try {
                std::string message;
                std::cout << "enter command (or 'quit' to exit): ";
                std::getline(std::cin, message);

                if (message == "quit") {
                    break;
                }

                send(message);
                std::string response = receive();
                std::cout << "server response: " << response << std::endl;
            } catch (std::exception& e) {
                std::cerr << "error: " << e.what() << std::endl;
                break;
            }
        }
    }

private:
    void send(const std::string& message) {
        boost::asio::write(socket_, boost::asio::buffer(message + "\n"));
    }

    std::string receive() {
        boost::asio::streambuf buf;
        boost::system::error_code error;
        boost::asio::read_until(socket_, buf, "\n", error);
        if (error) {
            throw std::runtime_error("read failed: " + error.message());
        }
        std::istream input(&buf);
        std::string response;
        std::getline(input, response);
        return response;
    }

    tcp::socket socket_;
    tcp::resolver::results_type endpoints_;
};

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "usage: client <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;
        Client client(io_context, argv[1], argv[2]);
        client.run();
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << "\n";
    }

    return 0;
}
#pragma once
#include <boost/asio.hpp>
#include <string>

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(boost::asio::ip::tcp::socket socket);
    void start();

private:
    void write_async(const std::string& s, std::function<void()> next);
    void read_line_async(std::function<void(std::string)> on_line);

    void stage_banner();
    void stage_login();
    void stage_password();
    void stage_shell();

    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf buf_;

    std::string ip_;
    int port_{0};

    std::string login_;
    std::string cwd_{"/home/admin"};
};

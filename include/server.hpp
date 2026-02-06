#pragma once
#include <boost/asio.hpp>

class Server {
public:
    explicit Server(int port);
    void run();

private:
    int port_;
};

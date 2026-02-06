#include "../include/server.hpp"
#include "../include/session.hpp"
#include "../include/logger.hpp"
#include "../include/discord_global.hpp"
#include "../include/discord_notifier.hpp"
#include "../include/discord_payload.hpp"
#include <iostream>
#include <cstdlib>

using boost::asio::ip::tcp;

Server::Server(int port) : port_(port) {}

void Server::run() {
    boost::asio::io_context io;
    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), port_));

    Logger::init("./logs");
    std::cout << "[*] Fake SSH honeypot listening on port " << port_ << "\n";

    std::function<void()> do_accept;
    do_accept = [&]() {
        acceptor.async_accept([&](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket))->start();
            }
            do_accept();
        });
    };

    do_accept();
    const char* url = std::getenv("DISCORD_WEBHOOK_URL");
    if (url && *url) {
        g_discord = std::make_shared<DiscordNotifier>(url);
        g_discord->enqueue(DiscordEvent{ make_startup_message_json("prod-bastion-01") });
    } else {
        std::cerr << "[DISCORD] DISCORD_WEBHOOK_URL not set\n";
    }
    io.run();
}

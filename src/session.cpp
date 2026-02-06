#include "../include/session.hpp"
#include "../include/utils.hpp"
#include "../include/fake_shell.hpp"
#include "../include/logger.hpp"
#include "../include/discord_global.hpp"
#include "../include/discord_payload.hpp"
#include "../include/discord_notifier.hpp"

using boost::asio::ip::tcp;

Session::Session(tcp::socket socket) : socket_(std::move(socket)) {
    auto ep = socket_.remote_endpoint();
    ip_ = ep.address().to_string();
    port_ = ep.port();
}

void Session::start() {
    Logger::connection(ip_, port_);
    if (g_discord) {
        g_discord->enqueue({ make_connect_embed_json("prod-bastion-01", ip_, port_) });
    }
    stage_banner();
}

void Session::write_async(const std::string& s, std::function<void()> next) {
    auto self = shared_from_this();
    boost::asio::async_write(socket_, boost::asio::buffer(s),
        [self, next](boost::system::error_code, std::size_t) {
            if (next) next();
        });
}

void Session::read_line_async(std::function<void(std::string)> on_line) {
    auto self = shared_from_this();
    boost::asio::async_read_until(socket_, buf_, "\n",
        [self, on_line](boost::system::error_code ec, std::size_t) {
            if (ec) return;
            std::istream is(&self->buf_);
            std::string line;
            std::getline(is, line);
            on_line(line);
        });
}

void Session::stage_banner() {
    write_async("SSH-2.0-OpenSSH_8.9p1 Ubuntu-3\r\n", [self = shared_from_this()] {
        self->stage_login();
    });
}

void Session::stage_login() {
    auto self = shared_from_this();
    write_async("login as: ", [self] {
        self->read_line_async([self](std::string l) {
            self->login_ = normalize_command(l);
            self->stage_password();
        });
    });
}

void Session::stage_password() {
    auto self = shared_from_this();
    write_async("Password: ", [self] {
        self->read_line_async([self](std::string) {
            self->write_async("\nWelcome to Ubuntu 22.04.3 LTS\n\n", [self] {
                self->stage_shell();
            });
        });
    });
}

void Session::stage_shell() {
    auto self = shared_from_this();
    std::string prompt = self->login_ + "@prod-bastion-01:" + self->cwd_ + "$ ";

    write_async(prompt, [self] {
        self->read_line_async([self](std::string raw) {
            std::string cmd = normalize_command(raw);
            Logger::command(self->ip_, self->login_, self->cwd_, cmd);

            std::string severity = "INFO";
            if (cmd.find("sudo") != std::string::npos ||
                cmd.find("nmap") != std::string::npos ||
                cmd.find("wget") != std::string::npos ||
                cmd.find("curl") != std::string::npos ||
                cmd.find("/etc/shadow") != std::string::npos) {
                severity = "ALERT";
            }

            if (g_discord && !cmd.empty()) {
                g_discord->enqueue(DiscordEvent{
                    make_cmd_embed_json(
                        "prod-bastion-01",
                        severity,
                        self->ip_,
                        self->port_,
                        self->login_,
                        self->cwd_,
                        cmd
                    )
                });
            }
            std::string out = Shell::handle(cmd, self->cwd_);
            if (out == "__EXIT__") {
                self->write_async("logout\n", []{});
                return;
            }
            self->write_async(out, [self] { self->stage_shell(); });
        });
    });
}

#include "../include/logger.hpp"
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <iostream>

static std::mutex g_mx;
static std::ofstream g_conn;
static std::ofstream g_cmd;

static std::string ts() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t = system_clock::to_time_t(now);
    std::tm tm{};
#if defined(__APPLE__)
    localtime_r(&t, &tm);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

namespace Logger {
    void init(const std::string& base_dir) {
        std::scoped_lock lk(g_mx);
        std::error_code ec;
        std::filesystem::create_directories(base_dir, ec);
        if (ec) {
            std::cerr << "[LOGGER] create_directories failed: " << ec.message() << "\n";
        }

        if (g_conn.is_open()) g_conn.close();
        if (g_cmd.is_open())  g_cmd.close();
        g_conn.clear();
        g_cmd.clear();

        const std::string conn_path = base_dir + "/connections.log";
        const std::string cmd_path  = base_dir + "/commands.log";

        std::cerr << "[LOGGER] CWD=" << std::filesystem::current_path() << "\n";
        std::cerr << "[LOGGER] base_dir=" << base_dir << "\n";
        std::cerr << "[LOGGER] conn_path=" << conn_path << "\n";
        std::cerr << "[LOGGER] cmd_path=" << cmd_path << "\n";

        if (std::filesystem::exists(conn_path) && !std::filesystem::is_regular_file(conn_path)) {
            std::cerr << "[LOGGER] ERROR: connections.log exists but is not a regular file\n";
        }
        if (std::filesystem::exists(cmd_path) && !std::filesystem::is_regular_file(cmd_path)) {
            std::cerr << "[LOGGER] ERROR: commands.log exists but is not a regular file\n";
        }

        errno = 0;
        g_conn.open(conn_path, std::ios::out | std::ios::app);

        if (!g_conn.is_open()) {
            std::cerr << "[LOGGER] open failed: " << conn_path
                      << " errno=" << errno << " (" << std::strerror(errno) << ")\n";
        }

        errno = 0;
        g_cmd.open(cmd_path, std::ios::out | std::ios::app);
        if (!g_cmd.is_open()) {
            std::cerr << "[LOGGER] open failed: " << cmd_path
                      << " errno=" << errno << " (" << std::strerror(errno) << ")\n";
        }

        if (g_conn.is_open()) g_conn.setf(std::ios::unitbuf);
        if (g_cmd.is_open())  g_cmd.setf(std::ios::unitbuf);
    }

    void connection(const std::string& ip, int port) {
        std::scoped_lock lk(g_mx);
        if (!g_conn.is_open()) return;

        g_conn << ts() << " CONNECT " << ip << ":" << port << "\n";
        g_conn.flush();
    }

    void command(const std::string& ip, const std::string& login,
             const std::string& cwd, const std::string& cmd) {
        std::scoped_lock lk(g_mx);
        if (!g_cmd.is_open()) return;

        g_cmd << ts() << " [" << ip << "] " << login
              << " " << cwd << " $ " << cmd << "\n";
        g_cmd.flush();
    }
}

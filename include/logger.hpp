#pragma once
#include <string>

namespace Logger {
    void init(const std::string& base_dir);
    void connection(const std::string& ip, int port);
    void command(const std::string& ip, const std::string& login,
                 const std::string& cwd, const std::string& cmd);
}

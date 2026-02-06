#pragma once
#include <string>

class Shell {
public:
    static std::string handle(const std::string& cmd, std::string& cwd);
};

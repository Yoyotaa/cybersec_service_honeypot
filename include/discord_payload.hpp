#pragma once
#include <string>

std::string json_escape(const std::string& s);

std::string make_startup_message_json(const std::string& host);

std::string make_connect_embed_json(
    const std::string& host,
    const std::string& ip,
    int port
);

std::string make_cmd_embed_json(
    const std::string& host,
    const std::string& severity, // "INFO"|"WARN"|"ALERT"
    const std::string& ip,
    int port,
    const std::string& login,
    const std::string& cwd,
    const std::string& cmd
);

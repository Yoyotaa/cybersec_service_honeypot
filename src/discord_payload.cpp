#include "../include/discord_payload.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

static std::string iso8601_utc_now() {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    std::tm tm{};
#if defined(__APPLE__)
    gmtime_r(&t, &tm);
#else
    gmtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::string json_escape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (unsigned char c : s) {
        switch (c) {
            case '\"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (c < 0x20) {
                    static const char* hex = "0123456789ABCDEF";
                    out += "\\u00";
                    out += hex[(c >> 4) & 0xF];
                    out += hex[c & 0xF];
                } else {
                    out += static_cast<char>(c);
                }
        }
    }
    return out;
}

static int color_for(const std::string& sev) {
    if (sev == "ALERT") return 16711680; // rouge
    if (sev == "WARN")  return 16753920; // orange
    return 65280; // vert
}

std::string make_startup_message_json(const std::string& host) {
    return std::string("{")
        + "\"username\":\"Bastion Honeypot\","
        + "\"content\":\"✅ Service started on **" + json_escape(host) + "**\""
        + "}";
}

std::string make_connect_embed_json(
    const std::string& host,
    const std::string& ip,
    int port
) {
    return std::string("{")
        + "\"username\":\"Bastion Honeypot\","
        + "\"embeds\":[{"
            "\"title\":\"🔌 CONNECT\","
            "\"description\":\"New connection received\","
            "\"color\":3447003," // bleu
            "\"fields\":["
                "{\"name\":\"Host\",\"value\":\"" + json_escape(host) + "\",\"inline\":true},"
                "{\"name\":\"IP\",\"value\":\"" + json_escape(ip) + "\",\"inline\":true},"
                "{\"name\":\"Port\",\"value\":\"" + std::to_string(port) + "\",\"inline\":true}"
            "],"
            "\"footer\":{\"text\":\"honeypot\"},"
            "\"timestamp\":\"" + iso8601_utc_now() + "\""
        "}]}";
}

std::string make_cmd_embed_json(
    const std::string& host,
    const std::string& severity,
    const std::string& ip,
    int port,
    const std::string& login,
    const std::string& cwd,
    const std::string& cmd
) {
    const int color = color_for(severity);
    const std::string title =
        (severity == "ALERT") ? "🚨 ALERT — Command"
      : (severity == "WARN")  ? "⚠️ WARN — Command"
                              : "✅ INFO — Command";

    return std::string("{")
        + "\"username\":\"Bastion Honeypot\","
        + "\"embeds\":[{"
            "\"title\":\"" + json_escape(title) + "\","
            "\"description\":\"```" + json_escape(cmd) + "```\","
            "\"color\":" + std::to_string(color) + ","
            "\"fields\":["
                "{\"name\":\"Host\",\"value\":\"" + json_escape(host) + "\",\"inline\":true},"
                "{\"name\":\"IP\",\"value\":\"" + json_escape(ip) + ":" + std::to_string(port) + "\",\"inline\":true},"
                "{\"name\":\"User\",\"value\":\"" + json_escape(login) + "\",\"inline\":true},"
                "{\"name\":\"CWD\",\"value\":\"" + json_escape(cwd) + "\",\"inline\":false}"
            "],"
            "\"footer\":{\"text\":\"honeypot\"},"
            "\"timestamp\":\"" + iso8601_utc_now() + "\""
        "}]}";
}

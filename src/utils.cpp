#include "../include/utils.hpp"

using boost::asio::ip::tcp;

std::string read_line(tcp::socket& socket)
{
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\n");
    std::istream is(&buf);
    std::string line;
    std::getline(is, line);
    return line;
}

void write(tcp::socket& socket, const std::string& data) {
    boost::asio::write(socket, boost::asio::buffer(data));
}

std::string normalize_command(std::string cmd)
{
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\r'), cmd.end());
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\n'), cmd.end());

    cmd.erase(
        cmd.begin(),
        std::find_if(cmd.begin(), cmd.end(),
                     [](unsigned char c) { return !std::isspace(c); })
    );

    cmd.erase(
        std::find_if(cmd.rbegin(), cmd.rend(),
                     [](unsigned char c) { return !std::isspace(c); })
            .base(),
        cmd.end()
    );

    std::transform(cmd.begin(), cmd.end(), cmd.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return cmd;
}

#pragma once
#include <boost/asio.hpp>
#include <string>

std::string read_line(boost::asio::ip::tcp::socket& socket);
void write(boost::asio::ip::tcp::socket& socket, const std::string& data);
std::string normalize_command(std::string cmd);

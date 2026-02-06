#pragma once
#include <string>
#include <map>
#include <vector>

bool dir_exists(const std::string& path);
std::vector<std::string> list_dir(const std::string& path);
std::string read_file(const std::string& path);
std::string join_path(const std::string& cwd, const std::string& dir);

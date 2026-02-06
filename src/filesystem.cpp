#include "../include/filesystem.hpp"

using FileTree = std::map<std::string, std::vector<std::string>>;

static FileTree fake_fs = {
    {"/", {"home", "etc", "var", "opt"}},
    {"/home", {"admin"}},
    {"/home/admin", {"backup.sh", "notes.txt"}},
    {"/etc", {"passwd"}},
    {"/var", {"log"}},
    {"/opt", {}}
};

static std::map<std::string, std::string> fake_files = {
    {"/etc/passwd",
     "root:x:0:0:root:/root:/bin/bash\n"
     "admin:x:1000:1000:Admin:/home/admin:/bin/bash\n"},

    {"/home/admin/backup.sh",
     "#!/bin/bash\n"
     "tar czf /backup/home.tar.gz /home\n"},

    {"/home/admin/notes.txt",
     "TODO:\n- rotate passwords\n- clean old logs\n"}
};

std::string join_path(const std::string& cwd, const std::string& dir)
{
    if (dir.empty() || dir == ".")
        return cwd;

    if (dir == "/")
        return "/";

    if (dir == "..") {
        if (cwd == "/") return "/";
        auto pos = cwd.find_last_of('/');
        return pos == 0 ? "/" : cwd.substr(0, pos);
    }

    if (dir.front() == '/')
        return dir;

    return cwd == "/" ? "/" + dir : cwd + "/" + dir;
}

bool dir_exists(const std::string& path)
{
    return fake_fs.find(path) != fake_fs.end();
}

std::vector<std::string> list_dir(const std::string& path)
{
    auto it = fake_fs.find(path);
    if (it == fake_fs.end())
        return {};
    return it->second;
}

std::string read_file(const std::string& path)
{
    auto it = fake_files.find(path);
    if (it == fake_files.end())
        return "";
    return it->second;
}

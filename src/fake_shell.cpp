#include "../include/fake_shell.hpp"
#include "../include/filesystem.hpp"

#include <string>

std::string Shell::handle(const std::string& cmd, std::string& cwd)
{
    if (cmd == "pwd") {
        return cwd + "\n";
    }
    if (cmd.rfind("cd", 0) == 0) {
        std::string arg = cmd.size() > 2 ? cmd.substr(3) : "";
        if (arg.empty()) {
            cwd = "/home/admin";
            return "";
        }
        std::string target = join_path(cwd, arg);
        if (dir_exists(target)) {
            cwd = target;
            return "";
        }
        return "bash: cd: " + arg + ": No such file or directory\n";
    }

    if (cmd == "ls") {
        auto entries = list_dir(cwd);
        std::string out;
        for (const auto& e : entries)
            out += e + " ";
        return out + "\n";
    }

    if (cmd.rfind("cat ", 0) == 0) {
        std::string file = cmd.substr(4);
        std::string path = join_path(cwd, file);
        std::string content = read_file(path);
        if (!content.empty())
            return content;
        return "cat: " + file + ": No such file\n";
    }

    if (cmd == "exit" || cmd == "logout") {
        return "__EXIT__";
    }

    if (cmd == "whoami") return "admin\n";

    if (cmd == "id") return "uid=1000(admin) gid=1000(admin) groups=1000(admin),27(sudo)\n";

    if (cmd == "uname" || cmd == "uname -s") return "Linux\n";

    if (cmd == "uname -a")
        return "Linux prod-bastion-01 5.15.0-86-generic x86_64 GNU/Linux\n";

    return "bash: " + cmd + ": command not found\n";
}


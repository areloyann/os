#pragma once
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iostream>

namespace mqutil {

inline std::string make_keyfile(const std::string& path) {
    int fd = open(path.c_str(), O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        throw std::runtime_error("Cannot open keyfile '" + path + "': " + std::string(strerror(errno)));
    }
    close(fd);
    return path;
}

inline key_t key_from_ftok(const std::string& path, int proj='M') {
    std::string f = make_keyfile(path);
    key_t k = ftok(f.c_str(), proj);
    if (k == -1) throw std::runtime_error(std::string("ftok failed: ") + strerror(errno));
    return k;
}

inline std::string errno_str() { return std::string(strerror(errno)); }

} // namespace mqutil

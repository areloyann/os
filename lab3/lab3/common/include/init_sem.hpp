#pragma once
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iostream>

// glibc requires defining union semun by user code
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

namespace semutil {

inline std::string make_keyfile(const std::string& path) {
    int fd = open(path.c_str(), O_CREAT | O_RDWR, 0666);
    if (fd == -1) throw std::runtime_error("Cannot open keyfile '" + path + "': " + std::string(strerror(errno)));
    close(fd);
    return path;
}

inline key_t key_from_ftok(const std::string& path, int proj='S') {
    std::string f = make_keyfile(path);
    key_t k = ftok(f.c_str(), proj);
    if (k == -1) throw std::runtime_error(std::string("ftok failed: ") + strerror(errno));
    return k;
}

inline std::string errno_str() { return std::string(strerror(errno)); }

} // namespace semutil

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
#include <vector>
#include <ctime>

// Define union semun for semctl
union semun {
    int              val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};

namespace semutil {

inline std::string make_keyfile(const std::string& path) {
    int fd = open(path.c_str(), O_CREAT | O_RDWR, 0666);
    if (fd == -1) throw std::runtime_error("Cannot open keyfile '" + path + "': " + std::string(strerror(errno)));
    close(fd);
    return path;
}

inline key_t key_from_ftok(const std::string& path, int proj='T') {
    std::string f = make_keyfile(path);
    key_t k = ftok(f.c_str(), proj);
    if (k == -1) throw std::runtime_error(std::string("ftok failed: ") + strerror(errno));
    return k;
}

inline std::string errno_str() { return std::string(strerror(errno)); }

inline unsigned long get_nsems(int semid) {
    union semun arg{};
    struct semid_ds ds{};
    arg.buf = &ds;
    if (semctl(semid, 0, IPC_STAT, arg) == -1) {
        throw std::runtime_error(std::string("semctl(IPC_STAT) failed: ") + strerror(errno));
    }
    return ds.sem_nsems;
}

inline time_t get_otime(int semid) {
    union semun arg{};
    struct semid_ds ds{};
    arg.buf = &ds;
    if (semctl(semid, 0, IPC_STAT, arg) == -1) return 0;
    return ds.sem_otime;
}

} // namespace semutil

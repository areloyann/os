#pragma once
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>
#include <iostream>

// Common helper to obtain a System V message queue ID.
// Default behavior: use ftok("./queue.key", 'M') and msgget with IPC_CREAT|0666.
// If ensure_new == true, create a brand-new queue using IPC_PRIVATE.
namespace mqutil {

inline std::string default_keyfile() { return "./queue.key"; }

inline void ensure_keyfile_exists(const std::string& path = default_keyfile()) {
    int fd = open(path.c_str(), O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        throw std::runtime_error("Failed to create/open keyfile '" + path + "': " + std::string(strerror(errno)));
    }
    close(fd);
}

inline int get_queue(bool ensure_new = false, int perms = 0666) {
    if (ensure_new) {
        // Create a brand-new queue not tied to a key.
        int msqid = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | perms);
        if (msqid == -1) {
            throw std::runtime_error(std::string("msgget(IPC_PRIVATE) failed: ") + strerror(errno));
        }
        return msqid;
    } else {
        ensure_keyfile_exists();
        key_t key = ftok(default_keyfile().c_str(), 'M');
        if (key == -1) {
            throw std::runtime_error(std::string("ftok failed: ") + strerror(errno));
        }
        int msqid = msgget(key, IPC_CREAT | perms);
        if (msqid == -1) {
            throw std::runtime_error(std::string("msgget failed: ") + strerror(errno));
        }
        return msqid;
    }
}

struct Message {
    long mtype;             // must be > 0
    char mtext[1024];       // generic buffer
};

inline size_t msize(const Message& m) {
    // Size parameter for msgsnd/msgrcv excludes the mtype field.
    return strnlen(m.mtext, sizeof(m.mtext));
}

} // namespace mqutil

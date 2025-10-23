#include "msg_file.hpp"
#include <sys/types.h>
#include <sys/wait.h>

static int open_or_die(const char* p) {
    int fd = open(p, O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error(std::string("open failed: ") + strerror(errno));
    }
    return fd;
}

int main() {
    try {
        key_t skey = mqutil::key_from_ftok(SERVER_KEY_PATH, SERVER_PROJ_ID);
        int server_qid = msgget(skey, IPC_CREAT | 0666);
        if (server_qid == -1) {
            std::perror("msgget(server)");
            return 1;
        }
        std::cout << "Server MQ ready. msqid=" << server_qid << "\n";

        while (true) {
            requestMsg req{};
            ssize_t n = msgrcv(server_qid, &req, sizeof(req) - sizeof(long), 0, 0);
            if (n == -1) {
                if (errno == EINTR) continue;
                std::perror("msgrcv(req)");
                break;
            }

            pid_t pid = fork();
            if (pid == -1) {
                std::perror("fork");
                continue;
            }
            if (pid == 0) {
                // child: handle request
                int client_qid = req.client_qid;
                responseMsg resp{};

                // Try open file
                int fd = open(req.pathname, O_RDONLY);
                if (fd == -1) {
                    resp.mtype = 3;
                    std::snprintf(resp.data, sizeof(resp.data), "open('%s'): %s", req.pathname, strerror(errno));
                    msgsnd(client_qid, &resp, sizeof(resp) - sizeof(long), 0);
                    _exit(0);
                }
                // Send file content in chunks
                while (true) {
                    ssize_t r = read(fd, resp.data, RESP_DATA_MAX-1);
                    if (r == -1) {
                        if (errno == EINTR) continue;
                        resp.mtype = 3;
                        std::snprintf(resp.data, sizeof(resp.data), "read error: %s", strerror(errno));
                        msgsnd(client_qid, &resp, sizeof(resp) - sizeof(long), 0);
                        close(fd);
                        _exit(0);
                    }
                    if (r == 0) break;
                    resp.mtype = 1;
                    // ensure NUL-terminated for safe printing; send r bytes+1
                    resp.data[r] = '\0';
                    msgsnd(client_qid, &resp, r+1, 0);
                }
                close(fd);
                // EOF marker
                resp.mtype = 2;
                resp.data[0] = '\0';
                msgsnd(client_qid, &resp, 1, 0);
                _exit(0);
            } else {
                // parent: optionally reap zombies
                int status;
                while (waitpid(-1, &status, WNOHANG) > 0) {}
            }
        }
        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Server fatal: %s\n", e.what());
        return 1;
    }
}

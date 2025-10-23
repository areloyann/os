#include "msg_file.hpp"

static void print_chunk(const char* s) {
    std::fwrite(s, 1, std::strlen(s), stdout);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "Usage: msg_file_client <pathname>\n");
        return 1;
    }
    const char* path = argv[1];
    try {
        // Create client queue (IPC_PRIVATE)
        int client_qid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
        if (client_qid == -1) { std::perror("msgget(client)"); return 1; }

        key_t skey = mqutil::key_from_ftok(SERVER_KEY_PATH, SERVER_PROJ_ID);
        int server_qid = msgget(skey, 0);
        if (server_qid == -1) {
            std::perror("msgget(server)");
            return 1;
        }

        // Send request
        requestMsg req{};
        req.mtype = 1;
        req.client_qid = client_qid;
        std::snprintf(req.pathname, sizeof(req.pathname), "%s", path);
        if (msgsnd(server_qid, &req, sizeof(req) - sizeof(long), 0) == -1) {
            std::perror("msgsnd(req)");
            return 1;
        }

        // Receive until EOF
        responseMsg resp{};
        while (true) {
            ssize_t n = msgrcv(client_qid, &resp, sizeof(resp) - sizeof(long), 0, 0);
            if (n == -1) {
                if (errno == EINTR) continue;
                std::perror("msgrcv(resp)");
                break;
            }
            if (resp.mtype == 1) {
                print_chunk(resp.data);
            } else if (resp.mtype == 2) {
                // EOF
                break;
            } else if (resp.mtype == 3) {
                std::fprintf(stderr, "\nServer error: %s\n", resp.data);
                break;
            }
        }

        // Cleanup client queue
        msgctl(client_qid, IPC_RMID, nullptr);
        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Client fatal: %s\n", e.what());
        return 1;
    }
}

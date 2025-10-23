#include "init_queue.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Task 2: Write a new message with mtype=20 and mtext="test".
// Optionally allow overriding via CLI: ./message_send [type] [text]
int main(int argc, char** argv) {
    try {
        int msqid = mqutil::get_queue(false);
        mqutil::Message msg{};
        msg.mtype = 20;
        const char* text = "test";
        if (argc >= 2) {
            long t = std::strtol(argv[1], nullptr, 10);
            if (t > 0) msg.mtype = t;
        }
        if (argc >= 3) {
            text = argv[2];
        }
        std::snprintf(msg.mtext, sizeof(msg.mtext), "%s", text);
        if (msgsnd(msqid, &msg, std::strlen(msg.mtext)+1, 0) == -1) {
            perror("msgsnd");
            return 1;
        }
        std::cout << "Sent: mtype=" << msg.mtype << ", mtext=\"" << msg.mtext << "\"\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}

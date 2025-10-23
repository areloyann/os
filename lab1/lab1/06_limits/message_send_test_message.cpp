#include "init_queue.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Task 10 (part 2): Try to send "Test message" after setting msg_qbytes=5.
// Without IPC_NOWAIT this may block; with IPC_NOWAIT it will fail with EAGAIN.
int main() {
    try {
        int msqid = mqutil::get_queue(false);
        mqutil::Message msg{};
        msg.mtype = 1;
        std::snprintf(msg.mtext, sizeof(msg.mtext), "%s", "Test message");
        size_t len = std::strlen(msg.mtext) + 1; // include NUL

        int flags = IPC_NOWAIT; // demonstrate immediate failure if not enough space
        if (msgsnd(msqid, &msg, len, flags) == -1) {
            perror("msgsnd");
            std::cerr << "Expected: If msg_qbytes is 5, and queue doesn't have enough free space, "
                         "IPC_NOWAIT causes EAGAIN instead of blocking.\n";
            return 1;
        }
        std::cout << "Sent: \"" << msg.mtext << "\"\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}

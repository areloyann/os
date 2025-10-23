#include "init_queue.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Task 6: Modify receiver to NOT block on empty queue (IPC_NOWAIT).
int main() {
    try {
        int msqid = mqutil::get_queue(false);
        mqutil::Message msg{};
        ssize_t n = msgrcv(msqid, &msg, sizeof(msg.mtext), 0, IPC_NOWAIT);
        if (n == -1) {
            if (errno == ENOMSG) {
                std::cout << "Queue is empty. (non-blocking)\n";
                return 0;
            } else {
                perror("msgrcv");
                return 1;
            }
        }
        std::cout << "Received: mtype=" << msg.mtype << ", mtext=\"" << msg.mtext << "\" (" << n << " bytes)\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}

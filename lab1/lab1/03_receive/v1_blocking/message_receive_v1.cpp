#include "init_queue.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Task 4: Drain queue (blocking). Then try one more read (will block).
// Usage: run this, then in another terminal send messages to see behavior.
int main() {
    try {
        int msqid = mqutil::get_queue(false);
        std::cout << "Receiving (blocking). msqid=" << msqid << "\n";
        mqutil::Message msg{};
        // Drain existing messages (block for the first one if none available).
        while (true) {
            ssize_t n = msgrcv(msqid, &msg, sizeof(msg.mtext), 0, 0); // 0 => blocking, any type
            if (n == -1) {
                perror("msgrcv");
                return 1;
            }
            std::cout << "Received: mtype=" << msg.mtype << ", mtext=\"" << msg.mtext << "\" (" << n << " bytes)\n";
            // Non-peeking drain: keep trying with IPC_NOWAIT until empty.
            while (true) {
                n = msgrcv(msqid, &msg, sizeof(msg.mtext), 0, IPC_NOWAIT);
                if (n == -1) {
                    if (errno == ENOMSG) {
                        std::cout << "Queue is now empty. Waiting for next message (will block)...\n";
                        break;
                    } else {
                        perror("msgrcv");
                        return 1;
                    }
                } else {
                    std::cout << "Received: mtype=" << msg.mtype << ", mtext=\"" << msg.mtext << "\" (" << n << " bytes)\n";
                }
            }
            // Now will loop and block waiting for next message
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}

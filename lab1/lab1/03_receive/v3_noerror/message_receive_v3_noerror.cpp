#include "init_queue.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Task 7: Use MSG_NOERROR so oversized messages are truncated instead of error.
int main() {
    try {
        int msqid = mqutil::get_queue(false);
        // Intentionally use a small buffer to demonstrate truncation.
        struct SmallMsg { long mtype; char mtext[8]; } small{};
        ssize_t n = msgrcv(msqid, &small, sizeof(small.mtext), 0, MSG_NOERROR);
        if (n == -1) {
            perror("msgrcv");
            return 1;
        }
        std::cout << "Received (possibly truncated): mtype=" << small.mtype
                  << ", mtext=\"" << small.mtext << "\" (" << n << " bytes into 8-byte buffer)\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}

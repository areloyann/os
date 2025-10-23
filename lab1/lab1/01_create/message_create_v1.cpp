#include "init_queue.h"
// Task 1: Run and explain the output (commented).
// This version creates (or opens if exists) a queue via ftok key.
int main() {
    try {
        int msqid = mqutil::get_queue(false);
        std::cout << "Queue created/opened. msqid=" << msqid << "\n";
        std::cout << "Use `ipcs -q` to verify it exists.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}

#include "init_queue.hpp"
// Task 8: Always create a new queue each run (IPC_PRIVATE).
int main() {
    try {
        int msqid = mqutil::get_queue(true);
        std::cout << "New queue created (always new). msqid=" << msqid << "\n";
        std::cout << "Note: Not associated with a key; pass msqid via output if another process needs it.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}

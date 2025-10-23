#include "init_queue.hpp"
#include <cstdlib>

// Task 9: Remove an existing queue.
// If no arg: remove the default (ftok) queue. If arg given: treat as msqid.
int main(int argc, char** argv) {
    try {
        int msqid;
        if (argc >= 2) {
            msqid = std::strtol(argv[1], nullptr, 10);
        } else {
            msqid = mqutil::get_queue(false);
        }
        if (msgctl(msqid, IPC_RMID, nullptr) == -1) {
            perror("msgctl(IPC_RMID)");
            return 1;
        }
        std::cout << "Removed queue msqid=" << msqid << "\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}

#include "init_queue.hpp"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cerrno>
#include <cstring>
#include <iostream>

// Task 10 (part 1): Set msg_qbytes to 5 bytes.
int main() {
    try {
        int msqid = mqutil::get_queue(false);
        struct msqid_ds ds{};
        if (msgctl(msqid, IPC_STAT, &ds) == -1) {
            perror("msgctl(IPC_STAT)");
            return 1;
        }
        // Set maximum bytes in queue to 5.
        ds.msg_qbytes = 5;
        if (msgctl(msqid, IPC_SET, &ds) == -1) {
            perror("msgctl(IPC_SET)");
            std::cerr << "Hint: You may need appropriate permissions or sysctl to allow lowering msg_qbytes.\n";
            return 1;
        }
        std::cout << "Set msqid=" << msqid << " msg_qbytes to " << ds.msg_qbytes << " bytes\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}

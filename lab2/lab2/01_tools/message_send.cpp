#include "init_queue.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

/*
Usage:
./message_send [-n] msqid msg-type [msg-text]

Options:
  -n   use IPC_NOWAIT flag

Example:
  ./message_send -n 0 10 Hello
*/

struct Msg { long mtype; char mtext[4096]; };

static void usage() {
    std::fprintf(stderr, "Usage: message_send [-n] msqid msg-type [msg-text]\n");
}

int main(int argc, char** argv) {
    bool nowait = false;
    std::vector<std::string> args;
    for (int i=1;i<argc;++i) {
        std::string a = argv[i];
        if (a == "-n") nowait = true;
        else args.push_back(a);
    }
    if (args.size() < 2) { usage(); return 1; }

    int msqid = std::strtol(args[0].c_str(), nullptr, 10);
    long mtype = std::strtol(args[1].c_str(), nullptr, 10);
    std::string text = (args.size() >= 3) ? args[2] : "";

    Msg msg{};
    msg.mtype = mtype > 0 ? mtype : 1;
    std::snprintf(msg.mtext, sizeof(msg.mtext), "%s", text.c_str());

    int flags = 0;
    if (nowait) flags |= IPC_NOWAIT;

    size_t len = std::strlen(msg.mtext) + 1;
    if (msgsnd(msqid, &msg, len, flags) == -1) {
        std::perror("msgsnd");
        return 1;
    }
    std::cout << "Sent: mtype=" << msg.mtype << ", size=" << len << "\n";
    return 0;
}

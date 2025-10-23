#include "init_queue.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

/*
Usage:
./message_receive [options] msqid [max-bytes]

Options:
  -e   MSG_NOERROR
  -n   IPC_NOWAIT
  -t type
  -x   MSG_EXCEPT

Example:
  ./message_receive -n 0 1024
*/

static void usage() {
    std::fprintf(stderr, "Usage: message_receive [options] msqid [max-bytes]\n");
}

struct AnyMsg { long mtype; char mtext[1]; };

int main(int argc, char** argv) {
    int flags = 0;
    long rtype = 0; // 0 => first message in queue
    std::vector<std::string> rest;

    for (int i=1;i<argc;++i) {
        std::string a = argv[i];
        if (a == "-e") flags |= MSG_NOERROR;
        else if (a == "-n") flags |= IPC_NOWAIT;
        else if (a == "-x") flags |= MSG_EXCEPT;
        else if (a == "-t") {
            if (i+1 >= argc) { usage(); return 1; }
            rtype = std::strtol(argv[++i], nullptr, 10);
        } else {
            rest.push_back(a);
        }
    }
    if (rest.size() < 1) { usage(); return 1; }

    int msqid = std::strtol(rest[0].c_str(), nullptr, 10);
    size_t maxbytes = (rest.size() >= 2) ? std::strtoul(rest[1].c_str(), nullptr, 10) : 8192;
    if (maxbytes == 0) maxbytes = 1;

    std::vector<char> buf(sizeof(long) + maxbytes);
    AnyMsg* msg = reinterpret_cast<AnyMsg*>(buf.data());

    ssize_t n = msgrcv(msqid, msg, maxbytes, rtype, flags);
    if (n == -1) {
        std::perror("msgrcv");
        return 1;
    }

    std::cout << "Received: mtype=" << msg->mtype << ", size=" << n << "\n";
    std::cout << "mtext: ";
    // Print up to n bytes safely
    for (ssize_t i=0;i<n;++i) {
        char c = msg->mtext[i];
        if (c == '\0') break;
        std::cout << c;
    }
    std::cout << "\n";
    return 0;
}

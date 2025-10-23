#include "init_queue.hpp"
#include <cstdio>
#include <cstdlib>
#include <vector>

/*
Usage:
./message_create [-cx] {-f pathname | -k key | -p} [octal-perms]

Options:
  -c            set IPC_CREAT
  -x            set IPC_EXCL
  -f pathname   use ftok(pathname, 'M')
  -k key        use numeric key (decimal)
  -p            use IPC_PRIVATE
  [octal-perms] e.g. 700, 666 (interpreted as octal)

Examples:
  ./message_create -c -p 700
*/

static void usage() {
    std::fprintf(stderr, "Usage: message_create [-cx] {-f pathname | -k key | -p} [octal-perms]\n");
}

int main(int argc, char** argv) {
    bool flag_c = false, flag_x = false;
    bool have_f = false, have_k = false, have_p = false;
    std::string f_path;
    key_t k_value = -1;
    int perms = 0666;

    int opt;
    // Minimal parser (no getopt dependency)
    for (int i=1; i<argc; ++i) {
        std::string a = argv[i];
        if (a == "-c") flag_c = true;
        else if (a == "-x") flag_x = true;
        else if (a == "-f") {
            if (i+1 >= argc) { usage(); return 1; }
            have_f = true;
            f_path = argv[++i];
        } else if (a == "-k") {
            if (i+1 >= argc) { usage(); return 1; }
            have_k = true;
            k_value = static_cast<key_t>(std::strtoll(argv[++i], nullptr, 10));
        } else if (a == "-p") {
            have_p = true;
        } else {
            // remaining argument should be perms (octal)
            perms = std::strtol(a.c_str(), nullptr, 8);
        }
    }

    // Validate key selection
    int key_count = (have_f?1:0) + (have_k?1:0) + (have_p?1:0);
    if (key_count != 1) {
        usage();
        std::fprintf(stderr, "Error: choose exactly one of -f, -k, or -p.\n");
        return 1;
    }

    int flags = 0;
    if (flag_c) flags |= IPC_CREAT;
    if (flag_x) flags |= IPC_EXCL;

    key_t key;
    if (have_p) {
        key = IPC_PRIVATE;
    } else if (have_k) {
        key = k_value;
    } else {
        try {
            key = mqutil::key_from_ftok(f_path, 'M');
        } catch (const std::exception& e) {
            std::fprintf(stderr, "Error: %s\n", e.what());
            return 1;
        }
    }

    int msqid = msgget(key, flags | perms);
    if (msqid == -1) {
        std::perror("msgget");
        return 1;
    }
    std::cout << "msqid=" << msqid << "\n";
    return 0;
}

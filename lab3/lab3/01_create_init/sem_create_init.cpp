#include "init_sem.hpp"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>

/*
Create a semaphore set with 1 semaphore and initialize it (default val=2).

Usage:
  ./sem_create_init [-cx] {-f pathname | -k key | -p} [octal-perms] [init-val]

Examples:
  ./sem_create_init -c -f sem.key 600 2
  ./sem_create_init -c -p 600 2
*/

static void usage() {
    std::fprintf(stderr, "Usage: sem_create_init [-cx] {-f pathname | -k key | -p} [octal-perms] [init-val]\n");
}

int main(int argc, char** argv) {
    bool flag_c=false, flag_x=false;
    bool have_f=false, have_k=false, have_p=false;
    std::string f_path;
    key_t k_value=-1;
    int perms = 0666;
    int init_val = 2;

    for (int i=1;i<argc;++i) {
        std::string a = argv[i];
        if (a=="-c") flag_c=true;
        else if (a=="-x") flag_x=true;
        else if (a=="-f") { if (i+1>=argc){usage(); return 1;} have_f=true; f_path=argv[++i]; }
        else if (a=="-k") { if (i+1>=argc){usage(); return 1;} have_k=true; k_value=static_cast<key_t>(std::strtoll(argv[++i],nullptr,10)); }
        else if (a=="-p") { have_p=true; }
        else if (perms==0666) { // first non-option -> perms (octal)
            char* end=nullptr;
            long v = std::strtol(a.c_str(), &end, 8);
            if (*end=='\0') { perms = static_cast<int>(v); }
            else { init_val = std::strtol(a.c_str(), nullptr, 10); }
        } else {
            init_val = std::strtol(a.c_str(), nullptr, 10);
        }
    }

    int key_count = (have_f?1:0)+(have_k?1:0)+(have_p?1:0);
    if (key_count!=1) { usage(); std::fprintf(stderr,"Error: choose exactly one of -f/-k/-p\n"); return 1; }

    int flags = 0;
    if (flag_c) flags |= IPC_CREAT;
    if (flag_x) flags |= IPC_EXCL;

    key_t key;
    if (have_p) key = IPC_PRIVATE;
    else if (have_k) key = k_value;
    else {
        try { key = semutil::key_from_ftok(f_path, 'S'); }
        catch (const std::exception& e){ std::fprintf(stderr,"%s\n", e.what()); return 1; }
    }

    int semid = semget(key, 1, flags | perms);
    if (semid==-1) { std::perror("semget"); return 1; }

    union semun arg{};
    arg.val = init_val;
    if (semctl(semid, 0, SETVAL, arg)==-1) { std::perror("semctl(SETVAL)"); return 1; }

    std::cout << "semid=" << semid << " init=" << init_val << "\n";
    return 0;
}

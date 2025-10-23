#include "init_sem.hpp"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>

/*
Initialize all semaphores in set using semctl(SETALL).

Usage:
  ./sem_setall semid v1 v2 v3 ... vN    # N must equal sem_nsems

Example:
  ./sem_setall 0 3 2 1 0
*/

static void usage(){
    std::fprintf(stderr,"Usage: sem_setall semid v1 v2 ... vN\n");
}

int main(int argc, char** argv){
    if (argc < 3){ usage(); return 1; }
    int semid = std::strtol(argv[1], nullptr, 10);

    unsigned long nsems = 0;
    try { nsems = semutil::get_nsems(semid); }
    catch (const std::exception& e){ std::fprintf(stderr,"%s\n", e.what()); return 1; }

    if ((unsigned long)(argc-2) != nsems){
        std::fprintf(stderr, "Error: need exactly %lu values\n", nsems);
        return 1;
    }
    std::vector<unsigned short> vals(nsems);
    for (unsigned long i=0; i<nsems; ++i){
        long v = std::strtol(argv[2+i], nullptr, 10);
        if (v < 0) v = 0;
        vals[i] = static_cast<unsigned short>(v);
    }

    union semun arg{};
    arg.array = vals.data();
    if (semctl(semid, 0, SETALL, arg)==-1){
        std::perror("semctl(SETALL)");
        return 1;
    }
    std::cout << "SETALL done for semid=" << semid << "\n";
    return 0;
}

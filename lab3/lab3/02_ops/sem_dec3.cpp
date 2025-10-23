#include "init_sem.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
/*
Try to decrement semaphore #0 by 3.
Usage:
  ./sem_dec3 [-n] semid
  -n  use IPC_NOWAIT (otherwise blocks)
Returns 0 on success; prints errors.
*/
int main(int argc, char** argv){
    bool nowait=false;
    int argi=1;
    for (; argi<argc; ++argi){
        std::string a=argv[argi];
        if (a=="-n") nowait=true;
        else break;
    }
    if (argi>=argc){ std::fprintf(stderr,"Usage: sem_dec3 [-n] semid\n"); return 1; }
    int semid = std::strtol(argv[argi], nullptr, 10);

    struct sembuf op{};
    op.sem_num = 0;
    op.sem_op  = -3;
    op.sem_flg = nowait ? IPC_NOWAIT : 0; // add SEM_UNDO if desired

    if (semop(semid, &op, 1)==-1){
        std::perror("semop(-3)");
        return 1;
    }
    std::puts("semop -3 OK");
    return 0;
}

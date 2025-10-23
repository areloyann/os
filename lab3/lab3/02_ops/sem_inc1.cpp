#include "init_sem.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
/*
Increment semaphore #0 by 1.
Usage:
  ./sem_inc1 semid
*/
int main(int argc, char** argv){
    if (argc<2){ std::fprintf(stderr,"Usage: sem_inc1 semid\n"); return 1; }
    int semid = std::strtol(argv[1], nullptr, 10);
    struct sembuf op{};
    op.sem_num=0;
    op.sem_op=+1;
    op.sem_flg=0;
    if (semop(semid, &op, 1)==-1){
        std::perror("semop(+1)");
        return 1;
    }
    std::puts("semop +1 OK");
    return 0;
}

#include "init_sem.hpp"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
/*
Show details of semaphore set.
Usage:
  ./sem_info semid
Prints GETVAL, GETPID, GETNCNT, GETZCNT and IPC_STAT fields.
*/
int main(int argc, char** argv){
    if (argc<2){ std::fprintf(stderr,"Usage: sem_info semid\n"); return 1; }
    int semid = std::strtol(argv[1], nullptr, 10);

    union semun arg{};
    int val = semctl(semid, 0, GETVAL, arg);
    if (val==-1){ std::perror("semctl(GETVAL)"); return 1; }
    int pid = semctl(semid, 0, GETPID, arg);
    if (pid==-1){ std::perror("semctl(GETPID)"); return 1; }
    int ncnt = semctl(semid, 0, GETNCNT, arg);
    if (ncnt==-1){ std::perror("semctl(GETNCNT)"); return 1; }
    int zcnt = semctl(semid, 0, GETZCNT, arg);
    if (zcnt==-1){ std::perror("semctl(GETZCNT)"); return 1; }

    struct semid_ds ds{};
    arg.buf = &ds;
    if (semctl(semid, 0, IPC_STAT, arg)==-1){ std::perror("semctl(IPC_STAT)"); return 1; }

    std::cout << "semid="<<semid<<"\n";
    std::cout << "  GETVAL  = " << val  << "\n";
    std::cout << "  GETPID  = " << pid  << "\n";
    std::cout << "  GETNCNT = " << ncnt << "\n";
    std::cout << "  GETZCNT = " << zcnt << "\n";
    std::cout << "  nsems   = " << ds.sem_nsems << "\n";
    std::cout << "  perms   = " << std::oct << (ds.sem_perm.mode & 0777) << std::dec << "\n";
    return 0;
}

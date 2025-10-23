#include "init_sem.hpp"
#include <cstdio>
#include <cstdlib>

/*
Print sem set info and all current values.
Usage:
  ./sem_info semid
*/
int main(int argc, char** argv){
    if (argc<2){ std::fprintf(stderr,"Usage: sem_info semid\n"); return 1; }
    int semid = std::strtol(argv[1], nullptr, 10);

    union semun arg{};
    struct semid_ds ds{};
    arg.buf = &ds;
    if (semctl(semid, 0, IPC_STAT, arg)==-1){ std::perror("semctl(IPC_STAT)"); return 1; }

    unsigned long n = ds.sem_nsems;
    std::vector<unsigned short> arr(n);
    arg.array = arr.data();
    if (semctl(semid, 0, GETALL, arg)==-1){ std::perror("semctl(GETALL)"); return 1; }

    std::cout << "semid="<<semid<<"\n";
    std::cout << "  nsems   = " << n << "\n";
    std::cout << "  sem_otime = " << ds.sem_otime << "\n";
    std::cout << "  sem_ctime = " << ds.sem_ctime << "\n";
    std::cout << "  perms   = " << std::oct << (ds.sem_perm.mode & 0777) << std::dec << "\n";
    for (unsigned long i=0;i<n;++i){
        int val = semctl(semid, i, GETVAL, arg);
        int pid = semctl(semid, i, GETPID, arg);
        int ncnt= semctl(semid, i, GETNCNT, arg);
        int zcnt= semctl(semid, i, GETZCNT, arg);
        std::cout << "  ["<<i<<"] val="<<val<<" pid="<<pid<<" ncnt="<<ncnt<<" zcnt="<<zcnt<<"\n";
    }
    return 0;
}

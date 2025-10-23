#include "init_sem.hpp"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

/*
Proper initialization demo to avoid race:
- If set doesn't exist, create it with N semaphores and pre-zero them via SETALL.
- Do a dummy semop (sem_op=0) to bump sem_otime without changing values.
- Then set final values with SETALL.

Second concurrent process detects sem_otime!=0 and SKIPS reinit.

Usage:
  ./sem_good_init {-f pathname | -k key} [N] [v1 ... vN]
  Defaults: N=4, values=3 2 1 0 (truncated/extended to N).

Run two instances in parallel to see messages.
*/

static void usage(){
    std::fprintf(stderr, "Usage: sem_good_init {-f pathname | -k key} [N] [v1 ... vN]\n");
}

int main(int argc, char** argv){
    bool have_f=false, have_k=false;
    std::string f_path;
    key_t k_value=-1;

    int argi=1;
    for (; argi<argc; ++argi){
        std::string a=argv[argi];
        if (a=="-f"){ if (argi+1>=argc){usage(); return 1;} have_f=true; f_path=argv[++argi]; }
        else if (a=="-k"){ if (argi+1>=argc){usage(); return 1;} have_k=true; k_value=static_cast<key_t>(std::strtoll(argv[++argi],nullptr,10)); }
        else break;
    }
    if (!(have_f ^ have_k)){ usage(); std::fprintf(stderr,"Error: choose exactly one of -f or -k\n"); return 1; }

    int N = 4;
    if (argi<argc) N = std::strtol(argv[argi++], nullptr, 10);
    if (N<=0) N=4;

    // default values 3,2,1,0...
    std::vector<unsigned short> vals(N);
    for (int i=0;i<N;++i) vals[i] = (i<4)? (unsigned short)(3-i) : 0;
    for (int i=0; argi<argc && i<N; ++i) {
        long v = std::strtol(argv[argi++], nullptr, 10);
        vals[i] = (v<0)?0:(unsigned short)v;
    }

    key_t key;
    if (have_k) key=k_value;
    else {
        try { key=semutil::key_from_ftok(f_path,'T'); }
        catch (const std::exception& e){ std::fprintf(stderr,"%s\n", e.what()); return 1; }
    }

    // Try create exclusively; if exists, open
    int semid = semget(key, N, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1){
        if (errno != EEXIST){ std::perror("semget(create)"); return 1; }
        semid = semget(key, 0, 0);
        if (semid == -1){ std::perror("semget(open)"); return 1; }

        time_t otime = semutil::get_otime(semid);
        std::cout << "[PID " << getpid() << "] existing set semid="<<semid<<" sem_otime="<<otime<<"\n";
        if (otime == 0){
            std::cout << "[PID " << getpid() << "] waiting for initialization...\n";
            for (int tries=0; tries<50; ++tries){
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                if (semutil::get_otime(semid) != 0) break;
            }
            std::cout << "[PID " << getpid() << "] sem_otime now="<<semutil::get_otime(semid)<<"\n";
        } else {
            std::cout << "[PID " << getpid() << "] initialization already completed. Skipping SETALL.\n";
        }
        return 0;
    }

    std::cout << "[PID " << getpid() << "] created new set semid="<<semid<<" nsems="<<N<<"\n";

    // Pre-zero via SETALL
    std::vector<unsigned short> zeros(N, 0);
    union semun arg{};
    arg.array = zeros.data();
    if (semctl(semid, 0, SETALL, arg)==-1){ std::perror("semctl(SETALL zeros)"); return 1; }

    // Dummy semop (sem_op=0) on sem#0 to set sem_otime without changing values
    struct sembuf op{};
    op.sem_num = 0;
    op.sem_op  = 0;           // wait-for-zero (immediate since value==0)
    op.sem_flg = 0;
    if (semop(semid, &op, 1)==-1){ std::perror("semop(0)"); /* continue anyway */ }

    std::cout << "[PID " << getpid() << "] sem_otime after dummy semop="<< semutil::get_otime(semid) << "\n";

    // Final SETALL to requested values
    arg.array = vals.data();
    if (semctl(semid, 0, SETALL, arg)==-1){ std::perror("semctl(SETALL vals)"); return 1; }
    std::cout << "[PID " << getpid() << "] initialized values via SETALL\n";

    return 0;
}

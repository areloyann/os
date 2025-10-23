#include "init_sem.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>

/*
Demonstrate SEM_UNDO effect.
Usage:
  ./sem_op_undo semid            # uses SEM_UNDO (decrement by 1 then exit)
  ./sem_op_undo --no-undo semid  # same but without SEM_UNDO
Check with sem_info before/after process exits.
*/
int main(int argc, char** argv){
    bool use_undo = true;
    int argi=1;
    if (argc>=2 && std::string(argv[1])=="--no-undo"){ use_undo=false; argi=2; }
    if (argc<=argi){ std::fprintf(stderr,"Usage: sem_op_undo [--no-undo] semid\n"); return 1; }
    int semid = std::strtol(argv[argi], nullptr, 10);

    struct sembuf op{};
    op.sem_num = 0;
    op.sem_op = -1; // decrement
    op.sem_flg = use_undo ? SEM_UNDO : 0;

    if (semop(semid, &op, 1)==-1){
        std::perror("semop(-1)");
        return 1;
    }
    std::cout << "Done semop(-1) " << (use_undo? "with":"without") << " SEM_UNDO. Now exit.\n";
    // Process exits immediately; if SEM_UNDO, kernel will roll back (+1).
    return 0;
}

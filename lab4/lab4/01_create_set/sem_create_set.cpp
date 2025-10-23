#include "init_sem.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>

/*
Create a semaphore set with given count (>=1).

Usage:
  ./sem_create_set [-c] {-f pathname | -k key | -p} <count> [octal-perms]

Examples:
  ./sem_create_set -c -f sem.key 4 666
  ./sem_create_set -c -p 4 700
*/

static void usage(){
    std::fprintf(stderr, "Usage: sem_create_set [-c] {-f pathname | -k key | -p} <count> [octal-perms]\n");
}

int main(int argc, char** argv){
    bool flag_c=false, have_f=false, have_k=false, have_p=false;
    std::string f_path;
    key_t k_value=-1;
    int count=-1;
    int perms=0666;

    int i=1;
    for (; i<argc; ++i){
        std::string a=argv[i];
        if (a=="-c") flag_c=true;
        else if (a=="-f") { if (i+1>=argc){usage(); return 1;} have_f=true; f_path=argv[++i]; }
        else if (a=="-k") { if (i+1>=argc){usage(); return 1;} have_k=true; k_value=static_cast<key_t>(std::strtoll(argv[++i], nullptr, 10)); }
        else if (a=="-p") { have_p=true; }
        else break;
    }
    if (i>=argc){ usage(); return 1; }
    count = std::strtol(argv[i++], nullptr, 10);
    if (count<=0){ std::fprintf(stderr,"count must be >=1\n"); return 1; }
    if (i<argc){ perms = std::strtol(argv[i], nullptr, 8); }

    int key_count=(have_f?1:0)+(have_k?1:0)+(have_p?1:0);
    if (key_count!=1){ usage(); std::fprintf(stderr,"choose exactly one of -f/-k/-p\n"); return 1; }

    key_t key;
    if (have_p) key=IPC_PRIVATE;
    else if (have_k) key=k_value;
    else {
        try { key=semutil::key_from_ftok(f_path,'T'); }
        catch (const std::exception& e){ std::fprintf(stderr,"%s\n", e.what()); return 1; }
    }

    int flags = flag_c ? IPC_CREAT : 0;
    int semid = semget(key, count, flags | perms);
    if (semid==-1){ std::perror("semget"); return 1; }

    std::cout << "semid=" << semid << " nsems=" << count << "\n";
    return 0;
}

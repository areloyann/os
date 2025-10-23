#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int opt;
    int flags = 0;
    key_t key = -1;
    int perms = 0666;

    while ((opt = getopt(argc, argv, "cf:k:px")) != -1) {
        switch (opt) {
            case 'c': flags |= IPC_CREAT; break;
            case 'x': flags |= IPC_EXCL; break;
            case 'p': key = IPC_PRIVATE; break;
            case 'k': key = atoi(optarg); break;
            default:
                fprintf(stderr, "Usage: %s [-cx] {-f pathname | -k key | -p} [octal-perms]\n", argv[0]);
                exit(1);
        }
    }

    if (optind < argc) {
        perms = strtol(argv[optind], NULL, 8);
    }

    if (key == -1) {
        fprintf(stderr, "Error: no key specified (use -f, -k or -p)\n");
        exit(1);
    }

    int msqid = msgget(key, flags | perms);
    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("Message queue created successfully.\n");
    printf("msqid = %d\n", msqid);

    return 0;
}


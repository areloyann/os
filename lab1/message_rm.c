#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <msqid1> [msqid2 ...]\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        int msqid = atoi(argv[i]);
        if (msgctl(msqid, IPC_RMID, NULL) == -1) {
            perror("msgctl");
        } else {
            printf("Message queue with ID %d removed.\n", msqid);
        }
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>

int main() {
    int msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("New message queue created with ID: %d\n", msqid);
    return 0;
}


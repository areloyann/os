#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>

int main() {
    key_t key;
    int msqid;

    key = ftok("mykeyfile.c", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    struct msqid_ds buf;

    if (msgctl(msqid, IPC_STAT, &buf) == -1) {
        perror("msgctl IPC_STAT");
        exit(1);
    }

    buf.msg_qbytes = 5;

    if (msgctl(msqid, IPC_SET, &buf) == -1) {
        perror("msgctl IPC_SET");
        exit(1);
    }

    printf("Max queue size (msg_qbytes) set to %lu bytes.\n", buf.msg_qbytes);
    return 0;
}

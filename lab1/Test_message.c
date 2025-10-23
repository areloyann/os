#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>

#define MSG_TEXT_SIZE 100

struct msg_buffer {
    long mtype;
    char mtext[MSG_TEXT_SIZE];
};

int main() {
    key_t key;
    int msqid;
    struct msg_buffer message;

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

    message.mtype = 1;
    strcpy(message.mtext, "Test message");

    if (msgsnd(msqid, &message, strlen(message.mtext)+1, IPC_NOWAIT) == -1) {
        perror("msgsnd");
        printf("Failed to send message. Queue may be full or max size exceeded.\n");
        exit(1);
    } else {
        printf("Message sent: %s\n", message.mtext);
    }

    return 0;
}


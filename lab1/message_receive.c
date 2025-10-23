#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>

#define MSG_TEXT_SIZE 4  

struct msg_buffer {
    long mtype;
    char mtext[100]; 
};

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

    struct msg_buffer message;

    ssize_t ret = msgrcv(msqid, &message, MSG_TEXT_SIZE, 0, IPC_NOWAIT | MSG_NOERROR);
    if (ret == -1) {
        if (errno == ENOMSG) {
            printf("Queue is empty, no messages available.\n");
        } else {
            perror("msgrcv");
        }
    } else {
        printf("Message received: type = %ld, text = %.*s\n", 
               message.mtype, (int)ret, message.mtext);
    }

    return 0;
}



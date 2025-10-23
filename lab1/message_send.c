#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define MSG_TEXT_SIZE 100

struct msg_buffer {
    long mtype;
    char mtext[MSG_TEXT_SIZE];
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
    message.mtype = 20; 
    strcpy(message.mtext, "test"); 

    if (msgsnd(msqid, &message, strlen(message.mtext)+1, 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    printf("Message sent: type = %ld, text = %s\n", message.mtype, message.mtext);
    return 0;
}

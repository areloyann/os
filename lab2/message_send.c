#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MAX_TEXT 512

struct mymsg {
    long mtype;
    char mtext[MAX_TEXT];
};

int main(int argc, char *argv[]) {
    int opt;
    int flags = 0;  
    int arg_index = 1;

    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        flags |= IPC_NOWAIT;
        arg_index = 2;
    }

    if (argc - arg_index < 3) {
        fprintf(stderr, "Usage: %s [-n] msqid msg-type msg-text\n", argv[0]);
        exit(1);
    }

    int msqid = atoi(argv[arg_index]);
    long mtype = atol(argv[arg_index + 1]);
    char *msg_text = argv[arg_index + 2];

    if (strlen(msg_text) >= MAX_TEXT) {
        fprintf(stderr, "Error: msg-text too long (max %d bytes)\n", MAX_TEXT-1);
        exit(1);
    }

    struct mymsg message;
    message.mtype = mtype;
    strncpy(message.mtext, msg_text, MAX_TEXT-1);
    message.mtext[MAX_TEXT-1] = '\0';  

    if (msgsnd(msqid, &message, strlen(message.mtext)+1, flags) == -1) {
        perror("msgsnd");
        exit(1);
    }

    printf("Message sent: type = %ld, text = %s\n", message.mtype, message.mtext);
    return 0;
}

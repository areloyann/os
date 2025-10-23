#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

#define MAX_TEXT 512

struct mymsg {
    long mtype;
    char mtext[MAX_TEXT];
};

int main(int argc, char *argv[]) {
    int opt;
    int flags = 0;
    long msg_type = 0;   
    int arg_index = 1;

    while (argc > arg_index && argv[arg_index][0] == '-') {
        if (strcmp(argv[arg_index], "-n") == 0) {
            flags |= IPC_NOWAIT;
        } else if (strcmp(argv[arg_index], "-e") == 0) {
            flags |= MSG_NOERROR;
        } else {
            fprintf(stderr, "Unknown option %s\n", argv[arg_index]);
            exit(1);
        }
        arg_index++;
    }

    if (argc - arg_index < 2) {
        fprintf(stderr, "Usage: %s [options] msqid msg-type [max-bytes]\n", argv[0]);
        exit(1);
    }

    int msqid = atoi(argv[arg_index]);
    msg_type = atol(argv[arg_index + 1]);
    size_t max_bytes = MAX_TEXT;

    if (argc - arg_index >= 3) {
        max_bytes = atol(argv[arg_index + 2]);
    }

    struct mymsg message;

    ssize_t ret = msgrcv(msqid, &message, max_bytes, msg_type, flags);
    if (ret == -1) {
        perror("msgrcv");
        exit(1);
    }

    printf("Message received: type = %ld, text = %s\n", message.mtype, message.mtext);
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include "msg_file.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int serverId = msgget(SERVER_KEY, 0); 
    if (serverId == -1) { perror("msgget"); exit(EXIT_FAILURE); }

    int clientId = msgget(IPC_PRIVATE, 0600); 
    if (clientId == -1) { perror("msgget"); exit(EXIT_FAILURE); }

    struct requestMsg req;
    req.mtype = 1;
    req.client_msqid = clientId;
    strncpy(req.pathname, argv[1], MAX_PATH-1);
    req.pathname[MAX_PATH-1] = '\0';

    if (msgsnd(serverId, &req, REQ_MSG_SIZE, 0) == -1) { perror("msgsnd"); exit(EXIT_FAILURE); }

    struct responseMsg resp;
    ssize_t msgLen;
    msgLen = msgrcv(clientId, &resp, RESP_MSG_SIZE, 0, 0);
    if (msgLen == -1) { perror("msgrcv"); exit(EXIT_FAILURE); }

    if (resp.mtype == RESP_MT_FAILURE) {
        printf("Server error: %s\n", resp.data);
    } else {
        printf("File content:\n%s\n", resp.data);
    }

    msgctl(clientId, IPC_RMID, NULL);

    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>
#include "msg_file.h"

int main()
{
    int serverId = msgget(SERVER_KEY, IPC_CREAT | 0600);
    if (serverId == -1) { perror("msgget"); exit(EXIT_FAILURE); }

    printf("Server is running...\n");

    while (1) {
        struct requestMsg req;
        if (msgrcv(serverId, &req, REQ_MSG_SIZE, 0, 0) == -1) {
            perror("msgrcv"); continue;
        }

        if (fork() == 0) { 
            struct responseMsg resp;
            FILE *f = fopen(req.pathname, "r");
            if (!f) {
                resp.mtype = RESP_MT_FAILURE;
                snprintf(resp.data, MAX_DATA, "Cannot open file: %s", req.pathname);
                msgsnd(req.client_msqid, &resp, RESP_MSG_SIZE, 0);
                exit(EXIT_SUCCESS);
            }

            resp.mtype = RESP_MT_DATA;
            fread(resp.data, 1, MAX_DATA-1, f);
            resp.data[MAX_DATA-1] = '\0';
            fclose(f);

            msgsnd(req.client_msqid, &resp, RESP_MSG_SIZE, 0);
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}

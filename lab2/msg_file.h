#ifndef MSG_FILE_H
#define MSG_FILE_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_KEY 1234  
#define MAX_PATH 256
#define MAX_DATA 512

struct requestMsg {
    long mtype;          
    int client_msqid;    
    char pathname[MAX_PATH]; 
};

#define RESP_MT_DATA 1
#define RESP_MT_FAILURE 2

struct responseMsg {
    long mtype;          
    char data[MAX_DATA]; 
};

#define REQ_MSG_SIZE (sizeof(struct requestMsg) - sizeof(long))
#define RESP_MSG_SIZE (sizeof(struct responseMsg) - sizeof(long))

#endif


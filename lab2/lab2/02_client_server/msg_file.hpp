#pragma once
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iostream>
#include "init_queue.hpp"

// Known server key via ftok on "server.key"
#define SERVER_KEY_PATH "server.key"
#define SERVER_PROJ_ID 'S'

// Message payload sizes
#define REQ_PATH_MAX 256
#define RESP_DATA_MAX 1024

// Request from client to server (into SERVER MQ)
struct requestMsg {
    long mtype;                // must be >0 (we'll use 1)
    int  client_qid;           // client's queue id
    char pathname[REQ_PATH_MAX];
};

// Response from server to client (into CLIENT MQ)
struct responseMsg {
    long mtype;                // 1=data chunk, 2=end/EOF, 3=error
    char data[RESP_DATA_MAX];  // for mtype=1: file bytes; for mtype=3: error text
};

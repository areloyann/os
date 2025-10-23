#define MAX_MTEXT 1024 /* Maximum message size in bytes */

struct mbuf {
    long mtype; /* Message type */
    char mtext[MAX_MTEXT]; /* Message body */
};

int getInt(const char* arg);

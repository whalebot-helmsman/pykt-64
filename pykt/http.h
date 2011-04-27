#ifndef HTTP_H
#define HTTP_H

#include "pykt.h"

#define CRLF "\r\n"
#define DELIM ": "

typedef struct iovec iovec_t;

typedef struct {
    int fd;
    iovec_t *iov;
    uint32_t iov_cnt;
    uint32_t iov_size;
    uint32_t total;
    uint32_t total_size;
    uint8_t sended;
} data_bucket;

inline int 
connect_socket(char *host, int port);

inline int 
writev_bucket(data_bucket *bucket);

#endif 



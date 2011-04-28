#ifndef PYKT_H
#define PYKT_H

#include <Python.h>

#include <assert.h>
#include <fcntl.h>   
#include <stddef.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>

#define VERSION 0.0.1

#ifdef DEVELOP
#define DEBUG(...) \
    do { \
        printf(__VA_ARGS__); \
        printf("\n"); \
    } while(0)
#else
#define DEBUG(...) do{}while(0)
#endif

extern PyObject *wait_callback;



#endif 

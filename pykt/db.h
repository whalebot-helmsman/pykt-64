#ifndef DB_H
#define DB_H

#include "pykt.h"


typedef struct {
    PyObject_HEAD
    int fd;
    char *host;
    int port;
    double timeout;

} DBObject;

extern PyTypeObject DBObjectType;


#endif

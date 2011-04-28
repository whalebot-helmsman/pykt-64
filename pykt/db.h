#ifndef DB_H
#define DB_H

#include "pykt.h"
#include "http.h"


typedef struct {
    PyObject_HEAD
    char *host;
    int port;
    double timeout;
    http_connection *con;

} DBObject;

extern PyTypeObject DBObjectType;


#endif

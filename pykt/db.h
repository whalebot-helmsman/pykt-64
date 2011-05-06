#ifndef DB_H
#define DB_H

#include "pykt.h"
#include "http.h"


typedef struct {
    PyObject_HEAD
    char *host;
    int port;
    http_connection *con;
    PyObject *dbObj;
} DBObject;

extern PyTypeObject DBObjectType;

inline int
is_opened(DBObject *self);

#endif

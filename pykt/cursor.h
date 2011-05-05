#ifndef CURSOR_H
#define CURSOR_H

#include "pykt.h"
#include "db.h"
#include "http.h"

typedef struct {
    PyObject_HEAD
    int cur;
    DBObject *db;
    PyObject* in_weakreflist;
} CursorObject;

extern PyTypeObject CursorObjectType;

#endif

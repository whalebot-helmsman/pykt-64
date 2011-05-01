#ifndef REST_H
#define REST_H

#include "pykt.h"
#include "db.h"
#include "http.h"


inline PyObject* 
rest_call_get(DBObject *db, PyObject *keyObj);

inline PyObject* 
rest_call_head(DBObject *db, PyObject *keyObj);

inline PyObject* 
rest_call_put(DBObject *db, PyObject *key, PyObject *value);

inline PyObject* 
rest_call_delete(DBObject *db, PyObject *keyObj);

#endif

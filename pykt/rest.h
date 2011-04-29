#ifndef REST_H
#define REST_H

#include "pykt.h"
#include "db.h"
#include "http.h"


inline PyObject* 
rest_call_put(DBObject *db, char *key, char *val);

#endif

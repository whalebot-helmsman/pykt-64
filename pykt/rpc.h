#ifndef RPC_H
#define RPC_H

#include "pykt.h"
#include "db.h"
#include "http.h"


inline PyObject* 
call_echo(DBObject *db);

#endif


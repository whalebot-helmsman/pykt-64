#ifndef RPC_H
#define RPC_H

#include "pykt.h"
#include "db.h"
#include "http.h"


inline PyObject* 
rpc_call_echo(DBObject *db);

inline PyObject* 
rpc_call_report(DBObject *db);

inline PyObject* 
rpc_call_status(DBObject *db);

inline PyObject* 
rpc_call_increment(DBObject *db, PyObject *keyObj, int num, int expire);

#endif


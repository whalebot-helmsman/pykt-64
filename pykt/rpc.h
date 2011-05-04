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
rpc_call_status(DBObject *db, PyObject *dbObj);

inline PyObject* 
rpc_call_clear(DBObject *db, PyObject *dbObj);

inline PyObject* 
rpc_call_add(DBObject *db, PyObject *keyObj, PyObject *valueObj, PyObject *dbObj, int expire);

inline PyObject* 
rpc_call_replace(DBObject *db, PyObject *keyObj, PyObject *valueObj, PyObject *dbObj, int expire);

inline PyObject* 
rpc_call_append(DBObject *db, PyObject *keyObj, PyObject *valueObj, PyObject *dbObj, int expire);

inline PyObject* 
rpc_call_increment(DBObject *db, PyObject *keyObj, PyObject *dbObj, int num, int expire);

inline PyObject* 
rpc_call_increment_double(DBObject *db, PyObject *keyObj, PyObject *dbObj, double num, int expire);

inline PyObject* 
rpc_call_cas(DBObject *db, PyObject *keyObj, PyObject *dbObj, PyObject *ovalObj, PyObject *nvalObj, int expire);

inline PyObject* 
rpc_call_match_prefix(DBObject *db, PyObject *dbObj, PyObject *prefixObj);

#endif


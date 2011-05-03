#include "db.h"
#include "rpc.h"
#include "rest.h"

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 1978
#define DEFAULT_TIMEOUT 30

static inline int
is_opened(DBObject *self)
{
    if(self->con){
        return 1;
    }
    PyErr_SetString(PyExc_IOError, "not opend database");
    return 0;
}

static inline PyObject *
DBObject_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    DBObject *self;

    self = (DBObject *)type->tp_alloc(type, 0);
    if(self == NULL){
        return NULL;
    }
    
    DEBUG("DBObject_new self %p", self);
    return (PyObject *)self;
}

static inline int
DBObject_init(DBObject *self, PyObject *args, PyObject *kwds)
{
    return 0;
}

static inline void
DBObject_dealloc(DBObject *self)
{
    DEBUG("DBObject_dealloc self %p", self);
    if(self->con){
        close_http_connection(self->con);
        self->con = NULL;
    }
    self->ob_type->tp_free((PyObject*)self);
}

static inline PyObject* 
DBObject_open(DBObject *self, PyObject *args, PyObject *kwargs)
{
    char *host = NULL;
    int port = 0;
    double timeout;
    http_connection *con;

    static char *kwlist[] = {"host", "port", "timeout", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|sid", kwlist, &host, &port, &timeout)){
        return NULL; 
    }
    
    DEBUG("DBObject_open self %p", self);
    DEBUG("DBObject_open args %s, %d", host, port);
    
    if(!host){
        host = DEFAULT_HOST;
    }
    if(port == 0){
        port = DEFAULT_PORT;
    }
    if(!timeout){
        timeout = DEFAULT_TIMEOUT;
    }
    con = open_http_connection(host, port);
    if(con == NULL){
        return NULL;
    }
    self->host = host;
    self->port = port;
    self->timeout = timeout;
    self->con = con;
	Py_INCREF(self);
    return (PyObject *)self;
}

static inline PyObject* 
DBObject_close(DBObject *self, PyObject *args)
{
    PyObject *result;
    result = Py_False;

    DEBUG("DBObject_close self %p", self);
    if(self->con){
        if(close_http_connection(self->con) > 0){
            self->con = NULL;
            result = Py_True;
        }
    }
    Py_INCREF(result);
    return result;

}

static inline PyObject* 
DBObject_echo(DBObject *self, PyObject *args)
{

    DEBUG("DBObject_echo self %p", self);
    if(!is_opened(self)){
        return NULL;
    }
    return rpc_call_echo(self);
}

static inline PyObject* 
DBObject_report(DBObject *self, PyObject *args)
{

    DEBUG("DBObject_report self %p", self);
    if(!is_opened(self)){
        return NULL;
    }
    return rpc_call_report(self);
}

static inline PyObject* 
DBObject_set(DBObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *key, *value;
    int expire = 0;

    static char *kwlist[] = {"key", "value", "expire", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|i", kwlist, &key, &value, &expire)){
        return NULL; 
    }
    if(!is_opened(self)){
        return NULL;
    }
    
    DEBUG("DBObject_set self %p", self);
    return rest_call_put(self, key, value, expire, MODE_SET);
}

static inline PyObject* 
DBObject_get(DBObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &key)){
        return NULL; 
    }
    if(!is_opened(self)){
        return NULL;
    }
    
    DEBUG("DBObject_get self %p", self);
    return rest_call_get(self, key);
}

static inline PyObject* 
DBObject_head(DBObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &key)){
        return NULL; 
    }
    if(!is_opened(self)){
        return NULL;
    }
    
    DEBUG("DBObject_head self %p", self);
    return rest_call_head(self, key);
}


static inline PyObject* 
DBObject_remove(DBObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &key)){
        return NULL; 
    }
    if(!is_opened(self)){
        return NULL;
    }
    
    DEBUG("DBObject_remove self %p", self);
    return rest_call_delete(self, key);
}


static inline PyObject* 
DBObject_status(DBObject *self, PyObject *args)
{
    PyObject *db_name = NULL;

    DEBUG("DBObject_status self %p", self);
    if (!PyArg_ParseTuple(args, "|O", &db_name)){
        return NULL;
    }
    if(!is_opened(self)){
        return NULL;
    }
    //DEBUG("optional db %s", db);
    return rpc_call_status(self, db_name);
}

static inline PyObject* 
DBObject_clear(DBObject *self, PyObject *args)
{
    PyObject *db_name = NULL;

    DEBUG("DBObject_clear self %p", self);
    if (!PyArg_ParseTuple(args, "|O", &db_name)){
        return NULL;
    }
    if(!is_opened(self)){
        return NULL;
    }
    return rpc_call_clear(self, db_name);
}

static inline PyObject* 
DBObject_add(DBObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *key, *value;
    int expire = 0;

    static char *kwlist[] = {"key", "value", "expire", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|i", kwlist, &key, &value, &expire)){
        return NULL; 
    }
    if(!is_opened(self)){
        return NULL;
    }
    
    DEBUG("DBObject_add self %p", self);
    return rest_call_put(self, key, value, expire, MODE_ADD);
    /*
    PyObject *key, *value, *db_name = NULL;
    int expire = 0;

    DEBUG("DBObject_add self %p", self);
    static char *kwlist[] = {"key", "value", "xt", "db", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|iO", kwlist, &key, &value, &expire, &db_name)){
        return NULL; 
    }
    if(!is_opened(self)){
        return NULL;
    }
    return rpc_call_add(self, key, value, db_name, expire);
    */
}

static inline PyObject* 
DBObject_replace(DBObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *key, *value;
    int expire = 0;

    static char *kwlist[] = {"key", "value", "expire", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|i", kwlist, &key, &value, &expire)){
        return NULL; 
    }
    if(!is_opened(self)){
        return NULL;
    }
    
    DEBUG("DBObject_replace self %p", self);
    return rest_call_put(self, key, value, expire, MODE_REPLACE);
    /*
    PyObject *key, *value, *db_name = NULL;
    int expire = 0;

    DEBUG("DBObject_replace self %p", self);
    static char *kwlist[] = {"key", "value", "xt", "db", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|iO", kwlist, &key, &value, &expire, &db_name)){
        return NULL; 
    }
    if(!is_opened(self)){
        return NULL;
    }
    return rpc_call_replace(self, key, value, db_name, expire);
    */
}

static inline PyObject* 
DBObject_append(DBObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *key, *value, *db_name = NULL;
    int expire = 0;

    DEBUG("DBObject_append self %p", self);
    static char *kwlist[] = {"key", "value", "expire", "db", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|iO", kwlist, &key, &value, &expire, &db_name)){
        return NULL; 
    }
    if(!is_opened(self)){
        return NULL;
    }
    return rpc_call_append(self, key, value, db_name, expire);
}

static inline PyObject* 
DBObject_increment(DBObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *key, *db_name = NULL;
    int num = 1, expire = 0;

    static char *kwlist[] = {"key", "num", "expire", "db", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iiO", kwlist, &key, &num, &expire, &db_name)){
        return NULL; 
    }
    if(!is_opened(self)){
        return NULL;
    }
    
    DEBUG("DBObject_increment self %p", self);
    return rpc_call_increment(self, key, db_name, num, expire);
}

static inline PyObject* 
DBObject_increment_double(DBObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *key, *db_name = NULL;
    int expire = 0;
    double num = 1.0;

    static char *kwlist[] = {"key", "num", "expire", "db", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|diO", kwlist, &key, &num, &expire, &db_name)){
        return NULL; 
    }
    if(!is_opened(self)){
        return NULL;
    }
    
    DEBUG("DBObject_increment_double self %p", self);
    return rpc_call_increment_double(self, key, db_name, num, expire);
}

static inline PyObject* 
DBObject_cas(DBObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject *key, *oval = NULL, *nval = NULL, *db = NULL;;
    int expire = 0;

    static char *kwlist[] = {"key", "oval", "nval", "expire", "db", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOiO", kwlist, &key, &oval, &nval, expire, db)){
        return NULL; 
    }
    if(!is_opened(self)){
        return NULL;
    }
    
    DEBUG("DBObject_cas self %p", self);
    return rpc_call_cas(self, key, db, oval, nval, expire);
}

static PyMethodDef DBObject_methods[] = {
    {"open", (PyCFunction)DBObject_open, METH_VARARGS|METH_KEYWORDS, 0},
    {"close", (PyCFunction)DBObject_close, METH_NOARGS, 0},
    {"get", (PyCFunction)DBObject_get, METH_VARARGS|METH_KEYWORDS, 0},
    {"head", (PyCFunction)DBObject_head, METH_VARARGS|METH_KEYWORDS, 0},
    {"set", (PyCFunction)DBObject_set, METH_VARARGS|METH_KEYWORDS, 0},
    {"remove", (PyCFunction)DBObject_remove, METH_VARARGS|METH_KEYWORDS, 0},
    {"echo", (PyCFunction)DBObject_echo, METH_NOARGS, 0},
    {"report", (PyCFunction)DBObject_report, METH_NOARGS, 0},
    {"status", (PyCFunction)DBObject_status, METH_VARARGS, 0},
    {"clear", (PyCFunction)DBObject_clear, METH_VARARGS, 0},
    {"add", (PyCFunction)DBObject_add, METH_VARARGS|METH_KEYWORDS, 0},
    {"replace", (PyCFunction)DBObject_replace, METH_VARARGS|METH_KEYWORDS, 0},
    {"append", (PyCFunction)DBObject_append, METH_VARARGS|METH_KEYWORDS, 0},
    {"increment", (PyCFunction)DBObject_increment, METH_VARARGS|METH_KEYWORDS, 0},
    {"increment_double", (PyCFunction)DBObject_increment_double, METH_VARARGS|METH_KEYWORDS, 0},
    {"cas", (PyCFunction)DBObject_cas, METH_VARARGS|METH_KEYWORDS, 0},
    {NULL, NULL}
};

PyTypeObject DBObjectType = {
	PyObject_HEAD_INIT(&PyType_Type)
    0,
    "pykt.KyotoTycoon",             /*tp_name*/
    sizeof(DBObject), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DBObject_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "KyotoTycoonObject",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    DBObject_methods,             /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)DBObject_init,                      /* tp_init */
    0,                         /* tp_alloc */
    DBObject_new,                           /* tp_new */
};


#include "db.h"

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 1978
#define DEFAULT_TIMEOUT 30


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

static PyMethodDef DBObject_methods[] = {
    {"open", (PyCFunction)DBObject_open, METH_VARARGS|METH_KEYWORDS, 0},
    {"close", DBObject_close, METH_NOARGS, 0},
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


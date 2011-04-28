#include "pykt.h"
#include "db.h"

PyObject *wait_callback;

PyObject *
set_wait_callback(PyObject *self, PyObject *args)
{
    PyObject *temp;
    if (!PyArg_ParseTuple(args, "O:watchdog", &temp))
        return NULL;
    
    if(!PyCallable_Check(temp)){
        PyErr_SetString(PyExc_TypeError, "must be callable");
        return NULL;
    }
    if(wait_callback){
        Py_DECREF(wait_callback);
    }
    wait_callback = temp;
    Py_INCREF(wait_callback);
    Py_RETURN_NONE;
}

static PyMethodDef PyKtMethods[] = {
    {"set_wait_callback", set_wait_callback, METH_VARARGS, "set wait callback"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


PyMODINIT_FUNC
initpykt(void)
{
    PyObject *m;
    
    if(PyType_Ready(&DBObjectType) < 0){ 
        return;
    }
    
    m = Py_InitModule3("pykt", PyKtMethods, "");
    if(m == NULL){
        return;
    }
    Py_INCREF(&DBObjectType);
    PyModule_AddObject(m, "KyotoTycoon", (PyObject *)&DBObjectType);

}

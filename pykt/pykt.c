#include "pykt.h"


static PyMethodDef PyKtMethods[] = {
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


PyMODINIT_FUNC
initpykt(void)
{
    PyObject *m;
    
    m = Py_InitModule3("pykt", PyKtMethods, "");
    if(m == NULL){
        return;
    }

}

#include "tsv.h"


inline void 
on_record(void *data, const char *key, size_t key_len, const char *val, size_t val_len)
{
    PyObject *keyObj, *valueObj, *dict;
    
    //DEBUG("on_record %.*s : %.*s ", key_len, key, val_len, val);

    tsv_ctx *ctx = (tsv_ctx *)data;
    dict = (PyObject *)ctx->user;
    keyObj = PyString_FromStringAndSize(key, key_len); 
    //DEBUG("on_record keyObj %p ", keyObj);
    if(keyObj == NULL){
        ctx->error = 1;
        PyErr_NoMemory();
        return ;
    }
    
    if(key == val || val_len == 0){
        valueObj = PyString_FromString("");
    }else{
        valueObj = PyString_FromStringAndSize(val, val_len); 
    }

    //DEBUG("on_record valueObj %p ", valueObj);
    if(valueObj == NULL){
        ctx->error = 1;
        PyErr_NoMemory();
        Py_DECREF(keyObj);
        return ;
    }
    if(PyDict_SetItem(dict, keyObj, valueObj) < 0){
        ctx->error = 1;
        Py_DECREF(keyObj);
        Py_DECREF(valueObj);
        return;
    }
    Py_DECREF(keyObj);
    Py_DECREF(valueObj);
}

inline PyObject * 
convert2dict(buffer *buf)
{
    tsv_ctx *ctx;
    PyObject *dict;
    size_t nread, off = 0;

    ctx = (tsv_ctx *)PyMem_Malloc(sizeof(tsv_ctx));
    if(ctx == NULL){
        PyErr_NoMemory();
        return NULL;
    }
    dict = PyDict_New();
    if(dict == NULL){
        PyMem_Free(ctx);
        PyErr_NoMemory();
        return NULL;
    }
    memset(ctx, 0, sizeof(tsv_ctx));
    tsv_init(ctx);
    ctx->on_record = on_record;
    ctx->user = dict;
    nread = tsv_execute(ctx, buf->buf, buf->len, off);
    
    if(ctx->error){
        //TODO Error
    }

    PyMem_Free(ctx);
    return (PyObject *)ctx->user;
}


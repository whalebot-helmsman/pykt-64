#include "rest.h"
#include "request.h"
#include "util.h"


inline PyObject* 
rest_call_put(DBObject *db, PyObject *key, PyObject *value)
{
    http_connection *con;
    data_bucket *bucket;
    char strlength[10];
    char *ckey, *cval, *encbuf;
    Py_ssize_t key_len, val_len;
    size_t encbuf_len;
    PyObject *result, *temp_key, *temp_val;
    
    result = Py_False;

    if(!PyString_Check(key)){
        PyErr_SetString(PyExc_TypeError, "key must be string ");
        return NULL;
    }
    if(value == Py_None){
        PyErr_SetString(PyExc_TypeError, "value is None");
        return NULL;
    }

    con = db->con;
    bucket = create_data_bucket(con->fd, 24);
    if(bucket == NULL){
        return NULL;
    }
    con->bucket = bucket;

    temp_key = PyString_AsEncodedObject(key, "utf-8", "strict");
    temp_val = PyObject_Str(value);
    
    PyString_AsStringAndSize(temp_key, &ckey, &key_len);
    PyString_AsStringAndSize(temp_val, &cval, &val_len);

    snprintf(strlength, sizeof (strlength), "%d", val_len);

    urlencode(cval, val_len, &encbuf, &encbuf_len);

    set_request_path(con, METHOD_PUT, LEN(METHOD_PUT), ckey, key_len);
    add_content_length(con, strlength, strlen(strlength));
    end_header(con);
    add_body(con, cval, val_len);
    
    if(request(con) > 0){
        result = Py_True;
    }
    
    PyMem_Free(encbuf);
    //free_data_bucket(bucket);
    //con->bucket = NULL;
    Py_DECREF(temp_key);
    Py_DECREF(temp_val);
    return result;
}

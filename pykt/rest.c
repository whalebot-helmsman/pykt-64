#include "rest.h"
#include "request.h"
#include "util.h"


inline PyObject* 
rest_call_get(DBObject *db, PyObject *keyObj)
{
    http_connection *con;
    data_bucket *bucket;
    char *key, *encbuf;
    Py_ssize_t key_len;
    size_t encbuf_len;
    PyObject *result = NULL;

    if(!PyString_Check(keyObj)){
        PyErr_SetString(PyExc_TypeError, "key must be string ");
        return NULL;
    }

    con = db->con;
    bucket = create_data_bucket(con->fd, 16);
    if(bucket == NULL){
        return NULL;
    }
    con->bucket = bucket;

    PyString_AsStringAndSize(keyObj, &key, &key_len);
    urlencode(key, key_len, &encbuf, &encbuf_len);

    set_request_path(con, METHOD_GET, LEN(METHOD_GET), encbuf, encbuf_len);
    end_header(con);
    
    if(request(con, 200) > 0){
        result = getPyString(con->response_body);
        DEBUG("response body %s", getString(con->response_body));
    }else{
        if(con->response_status == RES_SUCCESS){
            if(con->status_code == 404){
                result = Py_None;
                Py_INCREF(result);
            }
        }
    }
    
    free_http_data(con);
    PyMem_Free(encbuf);

    return result;
}

inline PyObject* 
rest_call_head(DBObject *db, PyObject *keyObj)
{
    http_connection *con;
    data_bucket *bucket;
    char *key, *encbuf;
    Py_ssize_t key_len;
    size_t encbuf_len;
    PyObject *result = NULL;

    if(!PyString_Check(keyObj)){
        PyErr_SetString(PyExc_TypeError, "key must be string ");
        return NULL;
    }

    con = db->con;
    bucket = create_data_bucket(con->fd, 16);
    if(bucket == NULL){
        return NULL;
    }
    con->bucket = bucket;

    PyString_AsStringAndSize(keyObj, &key, &key_len);
    urlencode(key, key_len, &encbuf, &encbuf_len);

    set_request_path(con, METHOD_HEAD, LEN(METHOD_HEAD), encbuf, encbuf_len);
    end_header(con);
    
    //head request
    con->head = 1;
    if(request(con, 200) > 0){
        result = Py_True;
        Py_INCREF(result);
    }else{
        if(con->response_status == RES_SUCCESS){
            if(con->status_code == 404){
                result = Py_False;
                Py_INCREF(result);
            }
        }
    }
    
    free_http_data(con);
    PyMem_Free(encbuf);

    return result;
}

inline PyObject* 
rest_call_put(DBObject *db, PyObject *keyObj, PyObject *valueObj)
{
    http_connection *con;
    data_bucket *bucket;
    char strlength[10];
    char *key, *val, *encbuf;
    Py_ssize_t key_len, val_len;
    size_t encbuf_len;
    PyObject *result = NULL, *temp_val;
    

    if(!PyString_Check(keyObj)){
        PyErr_SetString(PyExc_TypeError, "key must be string ");
        return NULL;
    }
    if(valueObj == Py_None){
        PyErr_SetString(PyExc_TypeError, "value is None");
        return NULL;
    }

    con = db->con;
    bucket = create_data_bucket(con->fd, 16);
    if(bucket == NULL){
        return NULL;
    }
    con->bucket = bucket;

    temp_val = PyObject_Str(valueObj);
    
    PyString_AsStringAndSize(keyObj, &key, &key_len);
    
    PyString_AsStringAndSize(temp_val, &val, &val_len);
    //get content-length str
    snprintf(strlength, sizeof (strlength), "%d", val_len);

    urlencode(key, key_len, &encbuf, &encbuf_len);
    DEBUG("urlencode key %s", encbuf);

    set_request_path(con, METHOD_PUT, LEN(METHOD_PUT), encbuf, encbuf_len);
    add_content_length(con, strlength, strlen(strlength));
    end_header(con);
    add_body(con, val, val_len);
    
    if(request(con, 201) > 0){
        result = Py_True;
        Py_INCREF(result);
    }else{
        if(con->response_status == RES_SUCCESS){
            result = Py_False;;
            Py_INCREF(result);
        }
    }
    
    free_http_data(con);
    PyMem_Free(encbuf);
    Py_DECREF(temp_val);
    return result;
}

inline PyObject* 
rest_call_delete(DBObject *db, PyObject *keyObj)
{
    http_connection *con;
    data_bucket *bucket;
    char *key, *encbuf;
    Py_ssize_t key_len;
    size_t encbuf_len;
    PyObject *result = NULL;

    if(!PyString_Check(keyObj)){
        PyErr_SetString(PyExc_TypeError, "key must be string ");
        return NULL;
    }

    con = db->con;
    bucket = create_data_bucket(con->fd, 16);
    if(bucket == NULL){
        return NULL;
    }
    con->bucket = bucket;

    PyString_AsStringAndSize(keyObj, &key, &key_len);
    urlencode(key, key_len, &encbuf, &encbuf_len);

    set_request_path(con, METHOD_DELETE, LEN(METHOD_DELETE), encbuf, encbuf_len);
    end_header(con);
    
    if(request(con, 204) > 0){
        result = Py_True;
        Py_INCREF(result);
    }else{
        if(con->response_status == RES_SUCCESS){
            if(con->status_code == 404){
                result = Py_False;
                Py_INCREF(result);
            }
        }
    }
    
    free_http_data(con);
    PyMem_Free(encbuf);

    return result;
}


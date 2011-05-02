#include "rpc.h"
#include "request.h"
#include "util.h"
#include "tsv.h"

#define ECHO_URL "/rpc/echo"
#define REPORT_URL "/rpc/report"
#define INCREMENT_URL "/rpc/increment"
#define STATUS_URL "/rpc/status"
#define CLEAR_URL "/rpc/clear"
#define ADD_URL "/rpc/add"
#define REPLACE_URL "/rpc/replace"
#define APPEND_URL "/rpc/append"

static inline int
set_error(http_connection *con)
{
    PyObject *dict, *temp;
    char *msg;

    dict = convert2dict(con->response_body);
    if(dict){
        temp = PyDict_GetItemString(dict, "ERROR");
        if(temp){
            msg = PyString_AsString(temp);
            if(msg){
                PyErr_SetString(KtException, msg);
                return 1;
            }else{
                //TypeError
                return -1;
            }

        }
    }
    PyErr_SetString(KtException, "could not set error ");
    
    return -1;
}

static inline PyObject*
get_num(http_connection *con)
{
    PyObject *dict, *temp;
    dict = convert2dict(con->response_body);
    if(dict){
        temp = PyDict_GetItemString(dict, "num");
        if(temp){
            return PyNumber_Int(temp);
        }
    }
    return NULL;

}

static inline int
init_bucket(http_connection *con, int size)
{
    data_bucket *bucket;

    bucket = create_data_bucket(con->fd, size);
    if(bucket == NULL){
        return -1;
    }
    con->bucket = bucket;
    return 1;
}


inline PyObject* 
rpc_call_echo(DBObject *db)
{
    http_connection *con;
    PyObject *result = NULL;

    con = db->con;
    
    if(init_bucket(con, 16) < 0){
        return NULL;
    }
    set_request_path(con, METHOD_POST, LEN(METHOD_POST), ECHO_URL, LEN(ECHO_URL));
    end_header(con);
    
    if(request(con, 200) > 0){
        result = Py_True;
        Py_INCREF(result);
    }else{
        if(con->response_status == RES_SUCCESS){
            result = Py_False;;
            Py_INCREF(result);
        }
    }
    
    free_http_data(con);

    return result;

}

inline PyObject* 
rpc_call_report(DBObject *db)
{

    http_connection *con;
    PyObject *result = NULL;

    con = db->con;
    if(init_bucket(con, 16) < 0){
        return NULL;
    }
    set_request_path(con, METHOD_POST, LEN(METHOD_POST), REPORT_URL, LEN(REPORT_URL));
    end_header(con);
    
    if(request(con, 200) > 0){
        result = convert2dict(con->response_body);
    }else{
        if(con->response_status == RES_SUCCESS){
            set_error(con);
        }else{
            PyErr_SetString(KtException, "could not set error ");
        }
    }
    
    free_http_data(con);

    return result;
}

inline PyObject* 
rpc_call_status(DBObject *db, char *db_name, Py_ssize_t db_len)
{

    http_connection *con;
    PyObject *result = NULL;
    char content_length[12];
    uint32_t body_len = 0;

    con = db->con;
    if(init_bucket(con, 16) < 0){
        return NULL;
    }

    if(db_name){
        body_len += 3 + db_len;
    }

    set_request_path(con, METHOD_POST, LEN(METHOD_POST), STATUS_URL, LEN(STATUS_URL));
    snprintf(content_length, sizeof (content_length), "%d", body_len);
    add_content_length(con, content_length, strlen(content_length));
    end_header(con);
    if(body_len > 0){
        add_body(con, "DB\t", 3);
        add_body(con, db_name, db_len);
    }
    
    if(request(con, 200) > 0){
        result = convert2dict(con->response_body);
    }else{
        if(con->response_status == RES_SUCCESS){
            set_error(con);
        }else{
            PyErr_SetString(KtException, "could not set error ");
        }
    }
    
    free_http_data(con);

    return result;
}

inline PyObject* 
rpc_call_clear(DBObject *db, char *db_name, Py_ssize_t db_len)
{

    http_connection *con;
    PyObject *result = NULL;
    char content_length[12];
    uint32_t body_len = 0;

    con = db->con;
    if(init_bucket(con, 16) < 0){
        return NULL;
    }

    if(db_name){
        body_len += 3 + db_len;
    }

    set_request_path(con, METHOD_POST, LEN(METHOD_POST), CLEAR_URL, LEN(CLEAR_URL));
    snprintf(content_length, sizeof (content_length), "%d", body_len);
    add_content_length(con, content_length, strlen(content_length));
    end_header(con);
    if(body_len > 0){
        add_body(con, "DB\t", 3);
        add_body(con, db_name, db_len);
    }
    
    if(request(con, 200) > 0){
        result = Py_True;
        Py_INCREF(result);
    }else{
        if(con->response_status == RES_SUCCESS){
            set_error(con);
        }else{
            PyErr_SetString(KtException, "could not set error ");
        }
    }
    
    free_http_data(con);

    return result;
}

static inline PyObject* 
add_internal(DBObject *db, char *url, size_t url_len, PyObject *keyObj, PyObject *valueObj, PyObject *dbObj, int expire)
{
    http_connection *con;
    char *key, *val, *encbuf, *db_name;
    Py_ssize_t key_len, val_len, db_name_len = 0;
    char content_length[12];
    char xt[14];
    uint64_t expire_time = 0;
    size_t encbuf_len, xt_len = 0;
    uint32_t body_len = 12;
    PyObject *result = NULL, *temp_val;

    DEBUG("rpc_call_add %p %p %p %d", keyObj, valueObj, dbObj, expire);
    if(!PyString_Check(keyObj)){
        PyErr_SetString(PyExc_TypeError, "key must be string ");
        return NULL;
    }
    if(dbObj && !PyString_Check(dbObj)){
        PyErr_SetString(PyExc_TypeError, "db must be string ");
        return NULL;
    }
    if(valueObj == Py_None){
        PyErr_SetString(PyExc_TypeError, "value is None");
        return NULL;
    }

    con = db->con;
    if(init_bucket(con, 24) < 0){
        return NULL;
    }
    
    temp_val = PyObject_Str(valueObj);
    
    PyString_AsStringAndSize(keyObj, &key, &key_len);
    PyString_AsStringAndSize(temp_val, &val, &val_len);
    
    urlencode(key, key_len, &encbuf, &encbuf_len);
    body_len += encbuf_len;
    body_len += val_len;

    if(dbObj){
        PyString_AsStringAndSize(temp_val, &db_name, &db_name_len);
        body_len += db_name_len;
        body_len += 5;
    }
    //expire -> expirestr
    if(expire > 0){
        expire_time = get_expire_time(expire);
        //set X-Kt-Kt
        snprintf(xt, sizeof (xt), "%llu", expire_time);
        xt_len = strlen(xt);
        body_len += xt_len;
        body_len += 5;
    }


    set_request_path(con, METHOD_POST, LEN(METHOD_POST), url, url_len);
    snprintf(content_length, sizeof (content_length), "%d", body_len);
    add_content_length(con, content_length, strlen(content_length));
    add_kt_content_type(con);
    end_header(con);
    
    if(db_name_len > 0){
        add_body(con, "DB\t", 3);
        add_body(con, db_name, db_name_len);
        add_body(con, CRLF, 2);
    }

    add_body(con, "key\t", 4);
    add_body(con, encbuf, encbuf_len);
    add_body(con, CRLF, 2);
    add_body(con, "value\t", 6);
    add_body(con, val, val_len);

    if(xt_len > 0){
        add_body(con, CRLF, 2);
        add_body(con, "xt\t", 3);
        add_body(con, xt, xt_len);
    }

    if(request(con, 200) > 0){
        result = Py_True;
        Py_INCREF(result);
    }else{
        if(con->response_status == RES_SUCCESS){
            set_error(con);
        }else{
            PyErr_SetString(KtException, "could not set error ");
        }
    }
    
    free_http_data(con);
    PyMem_Free(encbuf);
    Py_DECREF(temp_val);

    return result;
}

inline PyObject* 
rpc_call_add(DBObject *db, PyObject *keyObj, PyObject *valueObj, PyObject *dbObj, int expire)
{
    return add_internal(db, ADD_URL, LEN(ADD_URL), keyObj, valueObj, dbObj, expire);
}

inline PyObject* 
rpc_call_replace(DBObject *db, PyObject *keyObj, PyObject *valueObj, PyObject *dbObj, int expire)
{
    return add_internal(db, REPLACE_URL, LEN(REPLACE_URL), keyObj, valueObj, dbObj, expire);
}

inline PyObject* 
rpc_call_append(DBObject *db, PyObject *keyObj, PyObject *valueObj, PyObject *dbObj, int expire)
{
    return add_internal(db, APPEND_URL, LEN(APPEND_URL), keyObj, valueObj, dbObj, expire);
}

inline PyObject* 
rpc_call_increment(DBObject *db, PyObject *keyObj, int num, int expire)
{

    http_connection *con;
    char *key, *encbuf;
    char content_length[12];
    char addnum[12];
    char xt[14];
    uint64_t expire_time = 0;
    Py_ssize_t key_len;
    size_t encbuf_len, xt_len = 0, addnum_len;
    uint32_t body_len = 10;
    PyObject *result = NULL;

    if(!PyString_Check(keyObj)){
        PyErr_SetString(PyExc_TypeError, "key must be string ");
        return NULL;
    }

    con = db->con;
    if(init_bucket(con, 16) < 0){
        return NULL;
    }
    
    //url encode key
    PyString_AsStringAndSize(keyObj, &key, &key_len);
    urlencode(key, key_len, &encbuf, &encbuf_len);
    body_len += encbuf_len;

    //num -> strnum
    snprintf(addnum, sizeof (addnum), "%d", num);
    addnum_len = strlen(addnum);
    body_len += addnum_len;

    //expire -> expirestr
    if(expire > 0){
        expire_time = get_expire_time(expire);
        //set X-Kt-Kt
        snprintf(xt, sizeof (xt), "%llu", expire_time);
        //DEBUG("expire %s", xt);
        xt_len = strlen(xt);
        body_len += xt_len;
        body_len += 5;
    }


    set_request_path(con, METHOD_POST, LEN(METHOD_POST), INCREMENT_URL, LEN(INCREMENT_URL));
    //get content-length str
    snprintf(content_length, sizeof (content_length), "%d", body_len);
    add_content_length(con, content_length, strlen(content_length));
    add_kt_content_type(con);
    end_header(con);
    
    add_body(con, "key\t", 4);
    add_body(con, encbuf, encbuf_len);
    add_body(con, CRLF, 2);
    add_body(con, "num\t", 4);
    add_body(con, addnum, addnum_len);
    if(expire > 0){
        add_body(con, CRLF, 2);
        add_body(con, "xt\t", 3);
        add_body(con, xt, xt_len);
    }

    if(request(con, 200) > 0){
        result = get_num(con);
    }else{
        if(con->status_code == 450){
            set_error(con);
        }else{
            PyErr_SetString(KtException, "could not set error ");
        }
    }
    
    free_http_data(con);
    PyMem_Free(encbuf);

    return result;
}


#include "rpc.h"
#include "request.h"
#include "util.h"
#include "tsv.h"
#include "buffer.h"

#define BODY_BUF_SIZE 1024 * 8

#define ECHO_URL "/rpc/echo"
#define REPORT_URL "/rpc/report"
#define STATUS_URL "/rpc/status"
#define CLEAR_URL "/rpc/clear"
#define ADD_URL "/rpc/add"
#define REPLACE_URL "/rpc/replace"
#define APPEND_URL "/rpc/append"
#define INCREMENT_URL "/rpc/increment"
#define INCREMENT_DOUBLE_URL "/rpc/increment_double"
#define CAS_URL "/rpc/cas"
#define SET_BULK_URL "/rpc/set_bulk"
#define MATCH_PREFIX_URL "/rpc/match_prefix"
#define MATCH_REGEX_URL "/rpc/match_regex"



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
get_num(http_connection *con, uint8_t isdouble)
{
    PyObject *dict, *temp;
    dict = convert2dict(con->response_body);
    if(dict){
        temp = PyDict_GetItemString(dict, "num");
        if(temp){
            if(isdouble){
                return PyNumber_Float(temp);
            }else{
                return PyNumber_Int(temp);
            }
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

static inline int 
set_param_db(buffer *body, PyObject *dbObj)
{
    char *db_name;
    Py_ssize_t db_name_len;
    PyString_AsStringAndSize(dbObj, &db_name, &db_name_len);
    //TODO write error?
    write2buf(body, "DB\t", 3);
    write2buf(body, db_name, db_name_len);
    return 1;
}

static inline int 
set_param_key(buffer *body, PyObject *keyObj)
{
    char *key, *enckey;
    Py_ssize_t key_len;
    size_t enckey_len;

    PyString_AsStringAndSize(keyObj, &key, &key_len);
    urlencode(key, key_len, &enckey, &enckey_len);
    write2buf(body, "key\t", 4);
    write2buf(body, enckey, enckey_len);
    PyMem_Free(enckey);
    return 1;
}

static inline int 
set_param(buffer *body, char *key, size_t key_len, PyObject *valueObj)
{
    char *val;
    Py_ssize_t val_len;
    
    PyString_AsStringAndSize(valueObj, &val, &val_len);
    write2buf(body, key, key_len);
    write2buf(body, val, val_len);
    return 1;
}

static inline int 
set_param_value(buffer *body, char *key, size_t key_len, PyObject *valueObj)
{
    char *val;
    Py_ssize_t val_len;
    PyObject *temp_val;
    
    temp_val = serialize_value(valueObj);
    PyString_AsStringAndSize(temp_val, &val, &val_len);
    write2buf(body, key, key_len);
    write2buf(body, val, val_len);
    Py_DECREF(temp_val);
    return 1;
}

static inline int
set_param_xt(buffer *body, int expire)
{
    char xt[32];
    uint64_t expire_time = 0;
    size_t xt_len = 0;
    expire_time = get_expire_time(expire);
    snprintf(xt, sizeof (xt), "%llu", expire_time);
    xt_len = strlen(xt);
    write2buf(body, "xt\t", 3);
    write2buf(body, xt, xt_len);
    return 1;
}

static inline int
set_param_num_int(buffer *body, int num)
{
    char addnum[32];
    size_t addnum_len;

    //num -> strnum
    snprintf(addnum, sizeof (addnum), "%d", num);
    addnum_len = strlen(addnum);
    write2buf(body, "num\t", 4);
    write2buf(body, addnum, addnum_len);
    return 1;
}

static inline int
set_param_num_double(buffer *body, double num)
{
    char addnum[32];
    size_t addnum_len;

    //num -> strnum
    snprintf(addnum, sizeof (addnum), "%f", num);
    addnum_len = strlen(addnum);
    write2buf(body, "num\t", 4);
    write2buf(body, addnum, addnum_len);
    return 1;
}

static inline void
write_records(PyObject *dict, buffer *buf)
{
    PyObject *keyObj, *valueObj;
    Py_ssize_t size = 0, index = 1, pos = 0;

    size = PyDict_Size(dict);

    while (PyDict_Next(dict, &pos, &keyObj, &valueObj)) {
        char *key, *enckey, *val, *encval;
        Py_ssize_t key_len, val_len;
        size_t enckey_len, encval_len;
        PyObject *key_str = PyObject_Str(keyObj);
        PyObject *value_str = serialize_value(valueObj);
        
        //encode
        PyString_AsStringAndSize(key_str, &key, &key_len);
        urlencode(key, key_len, &enckey, &enckey_len);

        PyString_AsStringAndSize(value_str, &val, &val_len);
        urlencode(val, val_len, &encval, &encval_len);
        
        write2buf(buf, "_", 1);
        write2buf(buf, enckey, enckey_len);
        write2buf(buf, "\t", 1);
        write2buf(buf, encval, encval_len);
        if(index < size){
            write2buf(buf, CRLF, 2);
        }
        PyMem_Free(enckey);
        PyMem_Free(encval);

        Py_XDECREF(key_str);
        Py_XDECREF(value_str);
        index++;
    }
}

/**
static inline int
get_recods_size(PyObject *dict)
{
    PyObject *keyObj, *valueObj;
    Py_ssize_t pos = 0;

    while (PyDict_Next(dict, &pos, &keyObj, &valueObj)) {
        char *key, *enckey;
        Py_ssize_t key_len;
        size_t enckey_len;
        PyObject *key_str = PyObject_Str(keyObj);
        PyObject *value_str = PyObject_Str(valueObj);
        
        //encode
        PyString_AsStringAndSize(key_str, &key, &key_len);
        urlencode(key, key_len, &enckey, &enckey_len);
        PyMem_Free(enckey);

        Py_XDECREF(key_str);
        Py_XDECREF(value_str);
    }
    return 1;
}
*/

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
rpc_call_status(DBObject *db, PyObject *dbObj)
{

    http_connection *con;
    PyObject *result = NULL;
    char content_length[12];
    buffer *body;

    if(dbObj && !PyString_Check(dbObj)){
        PyErr_SetString(PyExc_TypeError, "db must be string ");
        return NULL;
    }

    con = db->con;
    body = new_buffer(128, 0);
    if(body == NULL){
        return NULL;
    }
    
    if(init_bucket(con, 16) < 0){
        return NULL;
    }

    if(dbObj && dbObj != Py_None){
        set_param_db(body, dbObj);
    }

    set_request_path(con, METHOD_POST, LEN(METHOD_POST), STATUS_URL, LEN(STATUS_URL));
    snprintf(content_length, sizeof (content_length), "%d", body->len);
    add_content_length(con, content_length, strlen(content_length));
    end_header(con);
    if(body->len > 0){
        add_body(con, body->buf, body->len);
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
    
    free_buffer(body);
    free_http_data(con);

    return result;
}

inline PyObject* 
rpc_call_clear(DBObject *db, PyObject *dbObj)
{

    http_connection *con;
    PyObject *result = NULL;
    char content_length[12];
    buffer *body;

    body = new_buffer(128, 0);
    if(body == NULL){
        return NULL;
    }

    con = db->con;
    if(init_bucket(con, 16) < 0){
        return NULL;
    }

    if(dbObj && dbObj != Py_None){
        set_param_db(body, dbObj);
    }

    set_request_path(con, METHOD_POST, LEN(METHOD_POST), CLEAR_URL, LEN(CLEAR_URL));
    snprintf(content_length, sizeof (content_length), "%d", body->len);
    add_content_length(con, content_length, strlen(content_length));
    end_header(con);
    if(body->len > 0){
        add_body(con, body->buf, body->len);
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
    
    free_buffer(body);
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
    
    temp_val = serialize_value(valueObj);
    
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
    if(expire > 0){
        expire_time = get_expire_time(expire);
        snprintf(xt, sizeof (xt), "%llu", expire_time);
        xt_len = strlen(xt);
        body_len += xt_len;
        body_len += 5;
    }

    set_request_path(con, METHOD_POST, LEN(METHOD_POST), url, url_len);
    snprintf(content_length, sizeof (content_length), "%d", body_len);
    add_content_length(con, content_length, strlen(content_length));
    add_header_oneline(con, KT_CONTENT_TYPE, LEN(KT_CONTENT_TYPE));
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

/*
static inline PyObject* 
add_internal(DBObject *db, char *url, size_t url_len, PyObject *keyObj, PyObject *valueObj, PyObject *dbObj, int expire)
{
    http_connection *con;
    char content_length[12];
    buffer *body;
    PyObject *result = NULL;

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
    body = new_buffer(BODY_BUF_SIZE, 0);
    if(body == NULL){
        return NULL;
    }
    if(init_bucket(con, 16) < 0){
        return NULL;
    }
    
    if(dbObj && dbObj != Py_None){
        set_param_db(body, dbObj);
        write2buf(body, CRLF, 2);
    }
    set_param_key(body, keyObj);
    write2buf(body, CRLF, 2);
    set_param(body, "value\t", 6, valueObj);
    if(expire > 0){
        write2buf(body, CRLF, 2);
        set_param_xt(body, expire); 
    }

    set_request_path(con, METHOD_POST, LEN(METHOD_POST), url, url_len);
    snprintf(content_length, sizeof (content_length), "%d", body->len);
    add_content_length(con, content_length, strlen(content_length));
    add_header_oneline(con, KT_CONTENT_TYPE, LEN(KT_CONTENT_TYPE));
    end_header(con);
    
    add_body(con, body->buf, body->len);

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
    
    free_buffer(body);
    free_http_data(con);

    return result;
}
*/

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
rpc_call_increment(DBObject *db, PyObject *keyObj, PyObject *dbObj, int num, int expire)
{

    http_connection *con;
    char content_length[12];
    PyObject *result = NULL;
    buffer *body;

    if(!PyString_Check(keyObj)){
        PyErr_SetString(PyExc_TypeError, "key must be string ");
        return NULL;
    }
    if(dbObj && !PyString_Check(dbObj)){
        PyErr_SetString(PyExc_TypeError, "db must be string ");
        return NULL;
    }

    con = db->con;
    body = new_buffer(BODY_BUF_SIZE, 0);
    if(body == NULL){
        return NULL;
    }
    if(init_bucket(con, 16) < 0){
        return NULL;
    }
    
    if(dbObj && dbObj != Py_None){
        set_param_db(body, dbObj);
        write2buf(body, CRLF, 2);
    }
    set_param_key(body, keyObj);
    write2buf(body, CRLF, 2);
    set_param_num_int(body, num);
    if(expire > 0){
        write2buf(body, CRLF, 2);
        set_param_xt(body, expire);
    }

    set_request_path(con, METHOD_POST, LEN(METHOD_POST), INCREMENT_URL, LEN(INCREMENT_URL));
    snprintf(content_length, sizeof (content_length), "%d", body->len);
    add_content_length(con, content_length, strlen(content_length));
    add_header_oneline(con, KT_CONTENT_TYPE, LEN(KT_CONTENT_TYPE));
    end_header(con);
    
    add_body(con, body->buf, body->len);

    if(request(con, 200) > 0){
        result = get_num(con, 0);
    }else{
        if(con->response_status == RES_SUCCESS){
            set_error(con);
        }else{
            PyErr_SetString(KtException, "could not set error ");
        }
    }
    
    free_buffer(body);
    free_http_data(con);
    return result;
}

inline PyObject* 
rpc_call_increment_double(DBObject *db, PyObject *keyObj, PyObject *dbObj, double num, int expire)
{
    http_connection *con;
    char content_length[12];
    buffer *body;
    PyObject *result = NULL;

    if(!PyString_Check(keyObj)){
        PyErr_SetString(PyExc_TypeError, "key must be string ");
        return NULL;
    }
    if(dbObj && !PyString_Check(dbObj)){
        PyErr_SetString(PyExc_TypeError, "db must be string ");
        return NULL;
    }

    con = db->con;
    body = new_buffer(BODY_BUF_SIZE, 0);
    if(body == NULL){
        return NULL;
    }
    if(init_bucket(con, 16) < 0){
        return NULL;
    }
    
    if(dbObj && dbObj != Py_None){
        set_param_db(body, dbObj);
        write2buf(body, CRLF, 2);
    }
    set_param_key(body, keyObj);
    write2buf(body, CRLF, 2);
    set_param_num_double(body, num);
    if(expire > 0){
        write2buf(body, CRLF, 2);
        set_param_xt(body, expire);
    }

    set_request_path(con, METHOD_POST, LEN(METHOD_POST), INCREMENT_DOUBLE_URL, LEN(INCREMENT_DOUBLE_URL));
    snprintf(content_length, sizeof (content_length), "%d", body->len);
    add_content_length(con, content_length, strlen(content_length));
    add_header_oneline(con, KT_CONTENT_TYPE, LEN(KT_CONTENT_TYPE));
    end_header(con);
    
    add_body(con, body->buf, body->len);

    if(request(con, 200) > 0){
        result = get_num(con, 1);
    }else{
        if(con->status_code == RES_SUCCESS){
            set_error(con);
        }else{
            PyErr_SetString(KtException, "could not set error ");
        }
    }
    
    free_buffer(body);
    free_http_data(con);

    return result;
}

inline PyObject* 
rpc_call_cas(DBObject *db, PyObject *keyObj, PyObject *dbObj, PyObject *ovalObj, PyObject *nvalObj, int expire)
{
    http_connection *con;
    char *key, *oval, *nval, *encbuf, *db_name;
    Py_ssize_t key_len, oval_len = 0, nval_len = 0, db_name_len = 0;
    char content_length[12];
    char xt[14];
    uint64_t expire_time = 0;
    size_t encbuf_len, xt_len = 0;
    uint32_t body_len = 4;
    PyObject *result = NULL, *ovalS = NULL, *nvalS = NULL;

    if(!PyString_Check(keyObj)){
        PyErr_SetString(PyExc_TypeError, "key must be string ");
        return NULL;
    }
    if(dbObj && !PyString_Check(dbObj)){
        PyErr_SetString(PyExc_TypeError, "db must be string ");
        return NULL;
    }

    con = db->con;
    if(init_bucket(con, 24) < 0){
        return NULL;
    }
    
    if(dbObj && dbObj != Py_None){
        PyString_AsStringAndSize(dbObj, &db_name, &db_name_len);
        body_len += db_name_len;
        body_len += 5;
    }
    
    if(ovalObj && ovalObj != Py_None){
        ovalS = serialize_value(ovalObj);
        PyString_AsStringAndSize(ovalS, &oval, &oval_len);
        body_len += oval_len;
        body_len += 7;
    }

    if(nvalObj && nvalObj != Py_None){
        nvalS = serialize_value(nvalObj);
        PyString_AsStringAndSize(nvalS, &nval, &nval_len);
        body_len += nval_len;
        body_len += 7;
    }
    
    PyString_AsStringAndSize(keyObj, &key, &key_len);
    
    urlencode(key, key_len, &encbuf, &encbuf_len);
    body_len += encbuf_len;

    if(expire > 0){
        expire_time = get_expire_time(expire);
        snprintf(xt, sizeof (xt), "%llu", expire_time);
        xt_len = strlen(xt);
        body_len += xt_len;
        body_len += 5;
    }

    set_request_path(con, METHOD_POST, LEN(METHOD_POST), CAS_URL, LEN(CAS_URL));
    snprintf(content_length, sizeof (content_length), "%d", body_len);
    add_content_length(con, content_length, strlen(content_length));
    add_header_oneline(con, KT_CONTENT_TYPE, LEN(KT_CONTENT_TYPE));
    end_header(con);
    
    if(db_name_len > 0){
        add_body(con, "DB\t", 3);
        add_body(con, db_name, db_name_len);
        add_body(con, CRLF, 2);
    }

    add_body(con, "key\t", 4);
    add_body(con, encbuf, encbuf_len);
    if(oval_len > 0){
        add_body(con, CRLF, 2);
        add_body(con, "oval\t", 5);
        add_body(con, oval, oval_len);
    }
    if(nval_len > 0){
        add_body(con, CRLF, 2);
        add_body(con, "nval\t", 5);
        add_body(con, nval, nval_len);
    }
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
    Py_XDECREF(ovalS);
    Py_XDECREF(nvalS);

    return result;

}

/* slow impl ...
inline PyObject* 
rpc_call_cas(DBObject *db, PyObject *keyObj, PyObject *dbObj, PyObject *ovalObj, PyObject *nvalObj, int expire)
{
    http_connection *con;
    char content_length[12];
    buffer *body;
    PyObject *result = NULL;

    if(!PyString_Check(keyObj)){
        PyErr_SetString(PyExc_TypeError, "key must be string ");
        return NULL;
    }
    if(dbObj && !PyString_Check(dbObj)){
        PyErr_SetString(PyExc_TypeError, "db must be string ");
        return NULL;
    }

    con = db->con;
    body = new_buffer(BODY_BUF_SIZE, 0);
    if(body == NULL){
        return NULL;
    }
    if(init_bucket(con, 16) < 0){
        return NULL;
    }
    
    if(dbObj && dbObj != Py_None){
        set_param_db(body, dbObj);
        write2buf(body, CRLF, 2);
    }
    set_param_key(body, keyObj);

    if(ovalObj && ovalObj != Py_None){
        write2buf(body, CRLF, 2);
        set_param(body, "oval\t", 5,  ovalObj);
    }

    if(nvalObj && nvalObj != Py_None){
        write2buf(body, CRLF, 2);
        set_param(body, "nval\t", 5, nvalObj);
    }

    if(expire > 0){
        write2buf(body, CRLF, 2);
        set_param_xt(body, expire);
    }

    set_request_path(con, METHOD_POST, LEN(METHOD_POST), CAS_URL, LEN(CAS_URL));
    snprintf(content_length, sizeof (content_length), "%d", body->len);
    add_content_length(con, content_length, strlen(content_length));
    add_header_oneline(con, KT_CONTENT_TYPE, LEN(KT_CONTENT_TYPE));
    end_header(con);
    
    add_body(con, body->buf, body->len);

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

}*/

inline PyObject* 
rpc_call_match_prefix(DBObject *db, PyObject *dbObj, PyObject *prefixObj)
{
    http_connection *con;
    char content_length[12];
    buffer *body;
    PyObject *result = NULL;

    if(dbObj && !PyString_Check(dbObj)){
        PyErr_SetString(PyExc_TypeError, "db must be string ");
        return NULL;
    }
    if(prefixObj && !PyString_Check(prefixObj)){
        PyErr_SetString(PyExc_TypeError, "prefix must be string ");
        return NULL;
    }

    con = db->con;
    body = new_buffer(BODY_BUF_SIZE, 0);
    if(body == NULL){
        return NULL;
    }
    if(init_bucket(con, 16) < 0){
        return NULL;
    }
    
    if(dbObj && dbObj != Py_None){
        set_param_db(body, dbObj);
        write2buf(body, CRLF, 2);
    }
    set_param(body, "prefix\t", 7, prefixObj);

    set_request_path(con, METHOD_POST, LEN(METHOD_POST), MATCH_PREFIX_URL, LEN(MATCH_PREFIX_URL));
    snprintf(content_length, sizeof (content_length), "%d", body->len);
    add_content_length(con, content_length, strlen(content_length));
    add_header_oneline(con, KT_CONTENT_TYPE, LEN(KT_CONTENT_TYPE));
    end_header(con);
    
    add_body(con, body->buf, body->len);

    if(request(con, 200) > 0){
        result = convert2keylist(con->response_body);
        //result = convert2dict(con->response_body);
    }else{
        if(con->status_code == RES_SUCCESS){
            set_error(con);
        }else{
            PyErr_SetString(KtException, "could not set error ");
        }
    }
    
    free_buffer(body);
    free_http_data(con);

    return result;
}

inline PyObject* 
rpc_call_match_regex(DBObject *db, PyObject *dbObj, PyObject *regexObj)
{
    http_connection *con;
    char content_length[12];
    buffer *body;
    PyObject *result = NULL;

    if(dbObj && !PyString_Check(dbObj)){
        PyErr_SetString(PyExc_TypeError, "db must be string ");
        return NULL;
    }
    if(regexObj && !PyString_Check(regexObj)){
        PyErr_SetString(PyExc_TypeError, "regex must be string ");
        return NULL;
    }

    con = db->con;
    body = new_buffer(BODY_BUF_SIZE, 0);
    if(body == NULL){
        return NULL;
    }
    if(init_bucket(con, 16) < 0){
        return NULL;
    }
    
    if(dbObj && dbObj != Py_None){
        set_param_db(body, dbObj);
        write2buf(body, CRLF, 2);
    }
    set_param(body, "regex\t", 6, regexObj);

    set_request_path(con, METHOD_POST, LEN(METHOD_POST), MATCH_REGEX_URL, LEN(MATCH_REGEX_URL));
    snprintf(content_length, sizeof (content_length), "%d", body->len);
    add_content_length(con, content_length, strlen(content_length));
    add_header_oneline(con, KT_CONTENT_TYPE, LEN(KT_CONTENT_TYPE));
    end_header(con);
    
    add_body(con, body->buf, body->len);

    if(request(con, 200) > 0){
        result = convert2keylist(con->response_body);
        //result = convert2dict(con->response_body);
    }else{
        if(con->status_code == RES_SUCCESS){
            set_error(con);
        }else{
            PyErr_SetString(KtException, "could not set error ");
        }
    }
    
    free_buffer(body);
    free_http_data(con);

    return result;
}


inline PyObject* 
rpc_call_set_bulk(DBObject *db, PyObject *recordObj, PyObject *dbObj, int expire, int atomic)
{

    http_connection *con;
    char content_length[12];
    PyObject *result = NULL;
    buffer *body;

    if(dbObj && !PyString_Check(dbObj)){
        PyErr_SetString(PyExc_TypeError, "db must be string ");
        return NULL;
    }
    if(recordObj && !PyDict_Check(recordObj)){
        PyErr_SetString(PyExc_TypeError, "record must be dict ");
        return NULL;
    }

    con = db->con;
    body = new_buffer(BODY_BUF_SIZE, 0);
    if(body == NULL){
        return NULL;
    }
    if(init_bucket(con, 24) < 0){
        return NULL;
    }

    if(dbObj && dbObj != Py_None){
        set_param_db(body, dbObj);
        write2buf(body, CRLF, 2);
    }
    if(expire > 0){
        set_param_xt(body, expire);
        write2buf(body, CRLF, 2);
    }
    if(atomic){
        write2buf(body, "atomic\t", 7);
        write2buf(body, "true", 4);
        write2buf(body, CRLF, 2);
    }
    write_records(recordObj, body);

    set_request_path(con, METHOD_POST, LEN(METHOD_POST), SET_BULK_URL, LEN(SET_BULK_URL));
    snprintf(content_length, sizeof (content_length), "%d", body->len);
    add_content_length(con, content_length, strlen(content_length));
    add_header_oneline(con, KT_CONTENT_TYPE, LEN(KT_CONTENT_TYPE));
    end_header(con);
    
    add_body(con, body->buf, body->len);

    if(request(con, 200) > 0){
        result = get_num(con, 0);
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


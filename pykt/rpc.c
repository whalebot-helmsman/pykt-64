#include "rpc.h"
#include "request.h"
#include "util.h"
#include "tsv.h"

#define ECHO_URL "/rpc/echo"
#define REPORT_URL "/rpc/report"
#define INCREMENT_URL "/rpc/increment"

inline PyObject* 
rpc_call_echo(DBObject *db)
{
    http_connection *con;
    data_bucket *bucket;
    PyObject *result = NULL;

    con = db->con;
    bucket = create_data_bucket(con->fd, 8);
    if(bucket == NULL){
        return NULL;
    }
    con->bucket = bucket;
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
    data_bucket *bucket;
    PyObject *result = NULL;

    con = db->con;
    bucket = create_data_bucket(con->fd, 8);
    if(bucket == NULL){
        return NULL;
    }
    con->bucket = bucket;
    set_request_path(con, METHOD_POST, LEN(METHOD_POST), REPORT_URL, LEN(REPORT_URL));
    end_header(con);
    
    if(request(con, 200) > 0){
        result = convert2dict(con->response_body);
        //DEBUG("response body %s", getString(con->response_body));
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
rpc_call_increment(DBObject *db, PyObject *keyObj, int num, int expire)
{

    http_connection *con;
    data_bucket *bucket;
    char *key, *encbuf;
    char content_length[12];
    char addnum[12];
    char xt[14];
    uint64_t expire_time = 0;
    Py_ssize_t key_len;
    size_t encbuf_len, xt_len = 0, addnum_len;
    uint32_t body_len = 10;
    PyObject *result = NULL, *dict = NULL, *temp = NULL ;

    if(!PyString_Check(keyObj)){
        PyErr_SetString(PyExc_TypeError, "key must be string ");
        return NULL;
    }

    con = db->con;
    bucket = create_data_bucket(con->fd, 24);
    if(bucket == NULL){
        return NULL;
    }
    con->bucket = bucket;
    
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
        dict = convert2dict(con->response_body);
        if(dict){
            temp = PyDict_GetItemString(dict, "num");
            if(temp){
                result = PyNumber_Int(temp);
            }
        }
        //DEBUG("response body %s", getString(con->response_body));
    }else{
        //TODO 450 (the existing record was not compatible).
        if(con->response_status == RES_SUCCESS){
            result = Py_False;;
            Py_INCREF(result);
        }
    }
    
    free_http_data(con);
    PyMem_Free(encbuf);

    return result;
}


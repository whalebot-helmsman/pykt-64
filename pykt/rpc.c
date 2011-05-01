#include "rpc.h"
#include "request.h"

#define ECHO_URL "/rpc/echo"
#define REPORT_URL "/rpc/report"

inline PyObject* 
rpc_call_echo(DBObject *db)
{
    http_connection *con;
    data_bucket *bucket;
    PyObject *result = NULL;

    con = db->con;
    bucket = create_data_bucket(con->fd, 24);
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
    bucket = create_data_bucket(con->fd, 24);
    if(bucket == NULL){
        return NULL;
    }
    con->bucket = bucket;
    set_request_path(con, METHOD_POST, LEN(METHOD_POST), REPORT_URL, LEN(REPORT_URL));
    end_header(con);
    
    if(request(con, 200) > 0){
        result = getPyString(con->response_body);
        DEBUG("response body %s", getString(con->response_body));
    }else{
        if(con->response_status == RES_SUCCESS){
            result = Py_False;;
            Py_INCREF(result);
        }
    }
    
    free_http_data(con);

    return result;
}


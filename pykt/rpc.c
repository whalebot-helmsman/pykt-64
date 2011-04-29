#include "rpc.h"

#define ECHO_URL "/rpc/echo"



inline PyObject* 
call_echo(DBObject *db)
{
    http_connection *con;
    data_bucket *bucket;
    PyObject *result = Py_False;

    con = db->con;
    bucket = create_data_bucket(con->fd, 24);
    if(bucket == NULL){
        return NULL;
    }
    con->bucket = bucket;
    set_request_path(con, METHOD_POST, LEN(METHOD_POST), ECHO_URL, LEN(ECHO_URL));
    end_header(con);
    
    if(request(con) > 0){
        result = Py_True;
    }
    return result;

}

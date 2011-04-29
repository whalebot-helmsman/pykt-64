#include "rpc.h"

#define ECHO_URL "/rpc/echo"



inline PyObject* 
call_echo(DBObject *db)
{
    http_connection *con;
    data_bucket *bucket;
    PyObject *result;

    con = db->con;
    bucket = create_data_bucket(con->fd, 24);
    if(bucket == NULL){
        return NULL;
    }
    con->bucket = bucket;
    set_request_path(con, METHOD_POST, sizeof(METHOD_POST)-1, ECHO_URL, sizeof(ECHO_URL)-1);
    add_header(con, "Host", 4, "127.0.0.1:1978", 14);
    add_header(con, "Accept-Encoding", 15, "identity", 8);
    end_header(con);
    
    request(con);
    return Py_True;

}

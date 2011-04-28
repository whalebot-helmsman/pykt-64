#include "rpc.h"

#define ECHO_URL "/rpc/echo"



inline PyObject* 
call_echo(DBObject *db)
{
    http_connection *con;
    con = open_http_connection(db->host, db->port);
    if(con == NULL){
        // 1. MemoryError
        // 2. IOError
        return NULL;
    }
    set_request_path(con, METHOD_POST, sizeof(METHOD_POST), ECHO_URL, sizeof(ECHO_URL));
    
    return NULL;

}

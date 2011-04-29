#include "rest.h"
#include "request.h"

inline PyObject* 
rest_call_put(DBObject *db, char *key, char *val)
{
    http_connection *con;
    data_bucket *bucket;
    char strlength[10];
    size_t val_len;
    PyObject *result = Py_False;

    con = db->con;
    bucket = create_data_bucket(con->fd, 24);
    if(bucket == NULL){
        return NULL;
    }
    con->bucket = bucket;
    val_len = strlen(val);

    snprintf(strlength, sizeof (strlength), "%d", val_len);
    
    set_request_path(con, METHOD_PUT, LEN(METHOD_PUT), key, strlen(key));
    add_content_length(con, strlength, strlen(strlength));
    end_header(con);
    add_body(con, val, val_len);
    
    if(request(con) > 0){
        result = Py_True;
    }
    return result;
}

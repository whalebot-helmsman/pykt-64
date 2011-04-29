#include "rest.h"


inline PyObject* 
rest_call_put(DBObject *db, char *key, char *val)
{

    http_connection *con;
    data_bucket *bucket;

    con = db->con;
    bucket = create_data_bucket(con->fd, 24);
    if(bucket == NULL){
        return NULL;
    }
    con->bucket = bucket;
    
    Py_RETURN_NONE;
}

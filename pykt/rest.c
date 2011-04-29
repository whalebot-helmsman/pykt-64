#include "rest.h"


inline PyObject* 
rest_call_put(DBObject *db, PyObject *key, PyObject *val)
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

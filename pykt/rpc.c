#include "rpc.h"

#define ECHO_URL "/rpc/echo"



inline PyObject* 
call_echo(DBObject *db){
    data_bucket *bucket;
    bucket = create_data_bucket(db->fd, 10);
    set2bucket(bucket, METHOD_POST, sizeof(METHOD_POST));
    set2bucket(bucket, ECHO_URL, sizeof(ECHO_URL));
    set2bucket(bucket, HTTP_10, sizeof(HTTP_10));
    newline2bucket(bucket);
    newline2bucket(bucket);
    
    return NULL;

}

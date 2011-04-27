#ifndef HTTP_H
#define HTTP_H

#include "pykt.h"
#include "db.h"
#include "bucket.h"

#define CRLF "\r\n"
#define DELIM ": "
#define SPACE " "

#define METHOD_GET "GET "
#define METHOD_POST "POST "
#define METHOD_PUT "PUT "
#define METHOD_DELETE "DELETE "

#define HTTP_10 " HTTP/1.0"
#define HTTP_11 " HTTP/1.1"

#define KT_CONTENT_TYPE "Content-Type : text/tab-separated-values; colenc=U"

#define KT_MODE_ADD "X-Kt-Mode : add" 
#define KT_MODE_REPLACE "X-Kt-Mode : replace" 

#define KT_FIELD_NAME "X-Kt-Xt" 


typedef struct {
    data_bucket *bucket;
} http_connection;

inline http_connection *
open_http_connection(DBObject *db);

inline void
request_http(http_connection *connection, char *method, size_t method_len, char *path, size_t path_len);


#endif 



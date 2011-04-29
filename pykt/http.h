#ifndef HTTP_H
#define HTTP_H

#include "pykt.h"
#include "bucket.h"
#include "http_parser.h"

#define CRLF "\r\n"
#define DELIM ": "
#define SPACE " "

#define METHOD_GET "GET "
#define METHOD_POST "POST "
#define METHOD_PUT "PUT "
#define METHOD_DELETE "DELETE "

#define HTTP_10 " HTTP/1.0"
#define HTTP_11 " HTTP/1.1"

#define CONTENT_LENGTH "Content-Length"

#define KT_CONTENT_TYPE "Content-Type : text/tab-separated-values; colenc=U"

#define KT_MODE_ADD "X-Kt-Mode : add" 
#define KT_MODE_REPLACE "X-Kt-Mode : replace" 

#define KT_FIELD_NAME "X-Kt-Xt" 


typedef struct {
    int fd;
    data_bucket *bucket;
    http_parser *parser;
    uint8_t response_complete;
} http_connection;

inline http_connection *
open_http_connection(const char *host, int port);

inline int
close_http_connection(http_connection *con);

inline void
set_request_path(http_connection *connection, char *method, size_t method_len, char *path, size_t path_len);

inline void
add_header(http_connection *connection, char *name, size_t name_len, char *value, size_t value_len);

inline void
end_header(http_connection *connection);

inline void
add_crlf(http_connection *connection);

inline int  
request(http_connection *connection);

#endif 



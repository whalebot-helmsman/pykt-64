#ifndef HTTP_H
#define HTTP_H

#include "pykt.h"
#include "bucket.h"
#include "buffer.h"
#include "http_parser.h"

#define CRLF "\r\n"
#define DELIM ": "
#define SPACE " "

#define METHOD_GET "GET "
#define METHOD_HEAD "HEAD "
#define METHOD_POST "POST "
#define METHOD_PUT "PUT "
#define METHOD_DELETE "DELETE "

#define HTTP_10 " HTTP/1.0\r\n"
#define HTTP_11 " HTTP/1.1\r\n"

#define CONTENT_LENGTH "Content-Length: "
#define KT_XT "X-Kt-Xt: " 

#define CONNECTION_KEEP_ALIVE "Connection: keep-alive\r\n"

#define KT_CONTENT_TYPE "Content-Type : text/tab-separated-values; colenc=U"

#define KT_MODE_ADD "X-Kt-Mode : add" 
#define KT_MODE_REPLACE "X-Kt-Mode : replace" 

#define KT_FIELD_NAME "X-Kt-Xt" 


typedef enum{
    RES_INIT = 0,
    RES_READY,
    RES_SUCCESS,
    RES_MEMORY_ERROR,
    RES_HTTP_ERROR,
} response_status_type;

typedef struct {
    int fd;
    data_bucket *bucket;
    http_parser *parser;
    buffer *response_body;
    int status_code;
    response_status_type response_status;
    uint8_t head;
} http_connection;

inline http_connection *
open_http_connection(const char *host, int port);

inline int
close_http_connection(http_connection *con);

inline void 
free_http_data(http_connection *con);

inline int  
request(http_connection *connection, int status_code);

#endif 



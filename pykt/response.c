#include "response.h"


static inline int
message_begin_cb(http_parser *p)
{
    return 0;
}

static inline int
header_field_cb (http_parser *p, const char *buf, size_t len, char partial)
{
    return 0;
}

static inline int
header_value_cb (http_parser *p, const char *buf, size_t len, char partial)
{
    return 0;
}

static inline int
request_path_cb (http_parser *p, const char *buf, size_t len, char partial)
{

    return 0;
}

static inline int
request_uri_cb (http_parser *p, const char *buf, size_t len, char partial)
{
    return 0;
}

static inline int
query_string_cb (http_parser *p, const char *buf, size_t len, char partial)
{
    return 0;
}

static inline int
fragment_cb (http_parser *p, const char *buf, size_t len, char partial)
{
    return 0;
}


static inline int
body_cb (http_parser *p, const char *buf, size_t len, char partial)
{
    return 0;
}

static inline int
headers_complete_cb (http_parser *p)
{
    return 0;
}

static inline int
message_complete_cb (http_parser *p)
{
    return 0;
}

static http_parser_settings settings =
  {.on_message_begin = message_begin_cb
  ,.on_header_field = header_field_cb
  ,.on_header_value = header_value_cb
  ,.on_path = request_path_cb
  ,.on_url = request_uri_cb
  ,.on_fragment = fragment_cb
  ,.on_query_string = query_string_cb
  ,.on_body = body_cb
  ,.on_headers_complete = headers_complete_cb
  ,.on_message_complete = message_complete_cb
  };

inline http_parser *  
init_parser(http_connection *con)
{
    http_parser *parser;
    parser = (http_parser *)PyMem_Malloc(sizeof(http_parser));
    if(parser == NULL){
        PyErr_NoMemory();
        return NULL;
    }

    memset(con->parser, 0, sizeof(http_parser));

    http_parser_init(parser, HTTP_RESPONSE);
    return parser;
}

inline size_t 
execute_parse(http_connection *con, const char *data, size_t len)
{

    return 0;
}

inline int 
parser_finish(http_connection *con)
{
    return 0;
}


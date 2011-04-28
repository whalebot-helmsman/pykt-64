#include "response.h"

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


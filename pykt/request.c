#include "request.h"

inline void
set_request_path(http_connection *con, char *method, size_t method_len, char *path, size_t path_len)
{
    data_bucket *bucket = con->bucket;

    set2bucket(bucket, method, method_len);
    set2bucket(bucket, path, path_len);
    set2bucket(bucket, HTTP_10, LEN(HTTP_10));
    set2bucket(bucket, CRLF, 2);
}

inline void
add_crlf(http_connection *con)
{
    set2bucket(con->bucket, CRLF, 2);
}

inline void
end_header(http_connection *con)
{
    add_crlf(con);
}

inline void
add_header(http_connection *con, char *name, size_t name_len, char *value, size_t value_len)
{
    data_bucket *bucket = con->bucket;
    set2bucket(bucket, name, name_len);
    set2bucket(bucket, DELIM, 2);
    set2bucket(bucket, value, value_len);
    set2bucket(bucket, CRLF, 2);
}


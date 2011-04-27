#include "http.h"


static inline int 
connect_socket(char *host, int port);

inline http_connection *
open_http_connection(DBObject *db)
{

    http_connection *connection;
    data_bucket *bucket;
    int fd;

    connection = PyMem_Malloc(sizeof(http_connection));
    if(connection == NULL){
        return NULL;
    }
    memset(connection, 0, sizeof(http_connection));
    fd = connect_socket(db->host, db->port);
    if(fd < 0){
        if(connection){
            PyMem_Free(connection);
        }
        return NULL;
    }
    bucket = create_data_bucket(fd, 32);
    if(bucket == NULL){
        if(connection){
            PyMem_Free(connection);
        }
        return NULL;
    }
    connection->bucket = bucket;
    return connection;
}

inline void
set_request_path(http_connection *connection, char *method, size_t method_len, char *path, size_t path_len)
{
    data_bucket *bucket = connection->bucket;
    set2bucket(bucket, method, method_len);
    set2bucket(bucket, path, path_len);
    set2bucket(bucket, HTTP_10, sizeof(HTTP_10));
}


static inline int 
connect_socket(char *host, int port)
{
    struct addrinfo hints, *res, *ai;
    int flag = 1;
    int err;
    int fd;
    char strport[7];

    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 
    
    snprintf(strport, sizeof (strport), "%d", port);
    
    Py_BEGIN_ALLOW_THREADS
    err = getaddrinfo(host, strport, &hints, &res);
    Py_END_ALLOW_THREADS

    if (err == -1) {
        PyErr_SetFromErrno(PyExc_IOError);
        return -1;
    }

    for(ai = res; ai != NULL; ai = ai->ai_next) {
        
        Py_BEGIN_ALLOW_THREADS
        fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        Py_END_ALLOW_THREADS
        
        if (fd < 0){
            return -1;
        }

        if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag,
                sizeof(int)) == -1) {
            close(fd);
            PyErr_SetFromErrno(PyExc_IOError);
            return -1;
        }
        
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag,
                sizeof(int)) == -1) {
            close(fd);
            PyErr_SetFromErrno(PyExc_IOError);
            return -1;
        }

        Py_BEGIN_ALLOW_THREADS
        err = connect(fd, ai->ai_addr, ai->ai_addrlen);
        Py_END_ALLOW_THREADS
        if (err < 0) {
            close(fd);
            fd = -1;
           continue;
        }
        break;
    }

    if (ai == NULL)  {
        close(fd);
        PyErr_SetString(PyExc_IOError,"failed to connect\n");
        return -1;
    }

    freeaddrinfo(res);
    return fd;
}

inline void
add_crlf(http_connection *connection)
{
    set2bucket(connection->bucket, CRLF, 2);
}

inline void
add_header(http_connection *connection, char *name, size_t name_len, char *value, size_t value_len)
{
    data_bucket *bucket = connection->bucket;
    set2bucket(bucket, name, name_len);
    set2bucket(bucket, DELIM, 2);
    set2bucket(bucket, value, value_len);
    set2bucket(bucket, CRLF, 2);
}


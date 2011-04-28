#include "http.h"
#include "response.h"

#define BUF_SIZE 1024 * 4

static inline int 
connect_socket(char *host, int port);

static inline int 
send_request(http_connection *con);

static inline int 
recv_data(http_connection *con);

static inline int 
recv_response(http_connection *con);

inline http_connection *
open_http_connection(DBObject *db)
{

    http_connection *con;
    data_bucket *bucket;
    int fd;

    con = PyMem_Malloc(sizeof(http_connection));
    if(con == NULL){
        PyErr_NoMemory();
        return NULL;
    }
    memset(con, 0, sizeof(http_connection));

    fd = connect_socket(db->host, db->port);
    if(fd < 0){
        if(con){
            //TODO IOError 
            close_http_connection(con);
        }
        return NULL;
    }

    bucket = create_data_bucket(fd, 24);
    if(bucket == NULL){
        if(con){
            //TODO IOError 
            close_http_connection(con);
        }
        return NULL;
    }
    con->bucket = bucket;
    con->fd = fd;
    return con;
}

inline void
close_http_connection(http_connection *con)
{
    if(con->bucket){
        free_data_bucket(con->bucket);
        con->bucket = NULL;
    }

    if(con->fd > 0){
        close(con->fd);
    }
    PyMem_Free(con);
}

inline void
set_request_path(http_connection *con, char *method, size_t method_len, char *path, size_t path_len)
{
    data_bucket *bucket = con->bucket;

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
add_crlf(http_connection *con)
{
    set2bucket(con->bucket, CRLF, 2);
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

inline PyObject * 
request(http_connection *con)
{
   int ret;
   ret = send_request(con);
   
   if(ret < 0){
       //error
   }
   ret = recv_response(con);
   return NULL;
}

static inline int 
send_request(http_connection *con)
{
    int ret;
    
    data_bucket *bucket = con->bucket;
    
    ret = writev_bucket(bucket);
    switch(ret){
        case 0:
            //EWOULDBLOCK or EAGAIN
            //TODO trampoline hook 
            break;
        case -1:
            //IO Error
            //TODO PyErr
            return -1;
        default:
            break;
    }
    return 1;
}

static inline int 
recv_response(http_connection *con)
{
    http_parser *parser;
    int ret;
    parser = init_parser(con);
    if(parser == NULL){
        //alloc error
        return -1;
    }
    con->parser = parser;
    con->parser->data = con;
    while(1){
        ret = recv_data(con);

    }
    return 1;
}

static inline int 
recv_data(http_connection *con)
{
    char buf[BUF_SIZE];
    ssize_t r;

    data_bucket *bucket = con->bucket;

    Py_BEGIN_ALLOW_THREADS
    r = read(bucket->fd, buf, sizeof(buf));
    Py_END_ALLOW_THREADS

    switch(r){
        case 0:
            //close  
            return 1;
        case -1:
            if (errno == EAGAIN || errno == EWOULDBLOCK) { /* try again later */
                //TODO trampoline
                break;
            } else {
                PyErr_SetFromErrno(PyExc_IOError);
                //TODO close connection
            }
            return -1;
        default:
            break;
    }
    execute_parse(con, buf, r);
    return 0;

}

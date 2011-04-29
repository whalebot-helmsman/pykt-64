#include "http.h"
#include "response.h"

#define BUF_SIZE 1024 * 4

static inline int 
connect_socket(const char *host, int port);

static inline int 
send_request(http_connection *con);

static inline int 
recv_data(http_connection *con);

static inline int 
recv_response(http_connection *con);

inline http_connection *
open_http_connection(const char *host, int port)
{

    http_connection *con;
    int fd;
    DEBUG("open_http_connection args %s:%d", host, port);

    con = PyMem_Malloc(sizeof(http_connection));
    if(con == NULL){
        PyErr_NoMemory();
        return NULL;
    }
    memset(con, 0, sizeof(http_connection));
    DEBUG("open_http_connection %p", con);

    fd = connect_socket(host, port);
    if(fd < 0){
        if(con){
            //TODO IOError 
            close_http_connection(con);
        }
        return NULL;
    }
    
    con->fd = fd;
    DEBUG("open  con fd:%d", con->fd);
    return con;
}

inline int
close_http_connection(http_connection *con)
{
    int ret = 0;
    DEBUG("close_http_connection %p", con);
    if(con->bucket){
        free_data_bucket(con->bucket);
        con->bucket = NULL;
    }

    if(con->fd > 0){
        close(con->fd);
        DEBUG("close con fd:%d", con->fd);
        con->fd = -1;
        ret = 1;
    }
    PyMem_Free(con);
    return ret;
}

inline void
set_request_path(http_connection *con, char *method, size_t method_len, char *path, size_t path_len)
{
    data_bucket *bucket = con->bucket;

    set2bucket(bucket, method, method_len);
    set2bucket(bucket, path, path_len);
    set2bucket(bucket, HTTP_10, LEN(HTTP_10));
    set2bucket(bucket, CRLF, 2);
}


static inline int 
connect_socket(const char *host, int port)
{
    struct addrinfo hints, *res, *ai;
    int flag = 1;
    int err;
    int fd;
    char strport[7];
    
    DEBUG("connect_socket %s:%d", host, port);
   
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 
    
    snprintf(strport, sizeof (strport), "%d", port);
    
    Py_BEGIN_ALLOW_THREADS
    err = getaddrinfo(host, strport, &hints, &res);
    Py_END_ALLOW_THREADS

    if (err == -1) {
        DEBUG("error getaddrinfo");
        PyErr_SetFromErrno(PyExc_IOError);
        return -1;
    }

    for(ai = res; ai != NULL; ai = ai->ai_next) {
        
        Py_BEGIN_ALLOW_THREADS
        fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        Py_END_ALLOW_THREADS
        
        if (fd == -1){
            continue;
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

inline int  
request(http_connection *con)
{
   int ret;

   DEBUG("request http_connection %p", con);

   ret = send_request(con);
   
   if(ret < 0){
       //error
       return ret;
   }
   return recv_response(con);
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
    DEBUG("sended request data. http_connection %p", con);
    return 1;
}

static inline int 
recv_response(http_connection *con)
{
    http_parser *parser;
    int ret;
    
    DEBUG("recv_response http_connection %p", con);
    
    parser = init_parser(con);
    if(parser == NULL){
        //alloc error
        return -1;
    }
    
    con->parser = parser;
    con->parser->data = con;
    while(1){
        ret = recv_data(con);
        if(ret > 0){
            //complete
            break;
        }
        if(ret < 0){
            //TODO Error
            goto error;
        }
    }
    
    DEBUG("response status code %d", parser->status_code);

    if(parser->status_code != 200){
        //TODO Error
        goto error;
    }
    if(con->parser){
        PyMem_Free(con->parser);
        con->parser = NULL;
    }
    
    return 1;
error:
    if(con->parser){
        PyMem_Free(con->parser);
        con->parser = NULL;
    }
    return -1;
}

static inline int 
recv_data(http_connection *con)
{
    char buf[BUF_SIZE];
    ssize_t r;
    int nread;

    data_bucket *bucket = con->bucket;

    Py_BEGIN_ALLOW_THREADS
    r = read(con->fd, buf, sizeof(buf));
    Py_END_ALLOW_THREADS

    DEBUG("read data fd:%d read:%d buf:%s", con->fd, r, buf);

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
    nread = execute_parse(con, buf, r);
    if(nread != r){
        //TODO error
        return -1;
    }
    if(parser_finish(con) > 0){
        //complete
        return 1;
    }
    return 0;

}

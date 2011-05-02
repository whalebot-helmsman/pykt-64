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
recv_response(http_connection *con, int status_code);

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
    con->response_status = RES_INIT;

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

inline void
free_http_data(http_connection *con)
{
    DEBUG("free_http_data %p", con);
    if(con->bucket){
        free_data_bucket(con->bucket);
        con->bucket = NULL;
    }
    if(con->response_body){
        free_buffer(con->response_body);
        con->response_body = NULL;
    }
}

inline int
close_http_connection(http_connection *con)
{
    int ret = 0;
    DEBUG("close_http_connection %p", con);
    
    free_http_data(con);

    if(con->fd > 0){
        close(con->fd);
        DEBUG("close con fd:%d", con->fd);
        con->fd = -1;
        ret = 1;
    }
    PyMem_Free(con);
    return ret;
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

inline int  
request(http_connection *con, int status_code)
{
   int ret;

   DEBUG("request http_connection %p", con);

   ret = send_request(con);
   
   if(ret < 0){
       //error
       return ret;
   }
   return recv_response(con, status_code);
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
            call_wait_callback(con->fd, WAIT_WRITE);
            return send_request(con);
        case -1:
            //IO Error
            return -1;
        default:
            break;
    }
    DEBUG("sended request data. http_connection %p", con);
    return 1;
}

static inline int 
recv_response(http_connection *con, int status_code)
{
    http_parser *parser;
    int ret;
    
    DEBUG("recv_response http_connection %p", con);

    parser = init_parser(con);
    if(parser == NULL){
        //alloc error
        return -1;
    }
    con->response_status = RES_READY;     
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
    
    if(parser->status_code == 400 ){
        //Invalid parameter
        PyErr_SetString(KtException,"Invalid parameter");
        goto error;
    }
    if(parser->status_code != status_code){
        
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

    Py_BEGIN_ALLOW_THREADS
    r = read(con->fd, buf, sizeof(buf));
    Py_END_ALLOW_THREADS

    DEBUG("read data fd:%d read:%d \n%.*s", con->fd, r, r, buf);

    switch(r){
        case 0:
            //close  
            return 1;
        case -1:
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                call_wait_callback(con->fd, WAIT_READ);
                return 0;
            } else {
                PyErr_SetFromErrno(PyExc_IOError);
            }
            con->response_status = RES_HTTP_ERROR;
            return -1;
        default:
            break;
    }
    nread = execute_parse(con, buf, r);
    if(nread != r){
        PyErr_SetString(PyExc_IOError,"HTTP response parse error");
        con->response_status = RES_HTTP_ERROR;
        return -1;
    }
    
    switch(con->response_status){
        case RES_SUCCESS:
            return 1;
        case RES_READY:
            return 0;
        default:
            return -1;
    }
    return 0;

}

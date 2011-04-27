#include "http.h"



inline int 
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

inline int 
writev_bucket(data_bucket *bucket){
    int ret;
    int i = 0;

    Py_BEGIN_ALLOW_THREADS
    ret = writev(bucket->fd, bucket->iov, bucket->iov_cnt);
    Py_END_ALLOW_THREADS
    
    switch(ret){
        case 0:
            bucket->sended = 1;
            return 1;
        case -1:
            //error
            if (errno == EAGAIN || errno == EWOULDBLOCK) { 
                // try again later
                return 0;
            }else{
                //ERROR
                PyErr_SetFromErrno(PyExc_IOError);
                return -1;
            }
            break;
        default:
            if(bucket->total > ret){
                for(; i < bucket->iov_cnt;i++){
                    if(ret > bucket->iov[i].iov_len){
                        //already write
                        ret -= bucket->iov[i].iov_len;
                        bucket->iov[i].iov_len = 0;
                    }else{
                        bucket->iov[i].iov_base += ret;
                        bucket->iov[i].iov_len = bucket->iov[i].iov_len - ret;
                        break;
                    }
                }
                bucket->total = bucket->total - ret;
                return writev_bucket(bucket);
            }
            bucket->sended = 1;

    }

    return 1;
}

static inline data_bucket *
create_data_bucket(int fd, int cnt){

    data_bucket *bucket;
    bucket = PyMem_Malloc(sizeof(data_bucket));
    memset(bucket, 0, sizeof(data_bucket));
    
    bucket->fd = fd;
    bucket->iov = (iovec_t *)PyMem_Malloc(sizeof(iovec_t) * cnt);
    bucket->iov_size = cnt;
    return bucket;
}

static inline void
free_data_bucket(data_bucket *bucket)
{
    PyMem_Free(bucket->iov);
    PyMem_Free(bucket);
}


static inline void
set2bucket(data_bucket *bucket, char *buf, size_t len)
{
    bucket->iov[bucket->iov_cnt].iov_base = buf;
    bucket->iov[bucket->iov_cnt].iov_len = len;
    bucket->iov_cnt++;
    bucket->total += len;
    bucket->total_size += len;
}


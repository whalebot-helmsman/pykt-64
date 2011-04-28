#include "bucket.h"

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

inline data_bucket *
create_data_bucket(int fd, int cnt)
{

    data_bucket *bucket;
    iovec_t *iov;

    bucket = PyMem_Malloc(sizeof(data_bucket));
    if(bucket == NULL){
        PyErr_NoMemory();
        return NULL;
    }
    memset(bucket, 0, sizeof(data_bucket));
    
    bucket->fd = fd;
    iov = (iovec_t *)PyMem_Malloc(sizeof(iovec_t) * cnt);
    if(iov == NULL){
        PyMem_Free(bucket);
        PyErr_NoMemory();
        return NULL;
    }
    bucket->iov = iov;
    bucket->iov_size = cnt;
    return bucket;
}

inline void
free_data_bucket(data_bucket *bucket)
{
    PyMem_Free(bucket->iov);
    PyMem_Free(bucket);
}

inline void
set2bucket(data_bucket *bucket, char *buf, size_t len)
{
    bucket->iov[bucket->iov_cnt].iov_base = buf;
    bucket->iov[bucket->iov_cnt].iov_len = len;
    bucket->iov_cnt++;
    bucket->total += len;
    bucket->total_size += len;
}




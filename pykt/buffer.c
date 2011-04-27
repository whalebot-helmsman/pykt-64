#include "buffer.h"

#define LIMIT_MAX 1024 * 1024 * 1024

#define MAXFREELIST 128

static buffer *buffer_free_list[MAXFREELIST];
static int numfree = 0;

inline void
buffer_list_fill(void)
{
    buffer *buf;
	while (numfree < MAXFREELIST) {
        buf = (buffer *)PyMem_Malloc(sizeof(buffer));
		buffer_free_list[numfree++] = buf;
	}
}

inline void
buffer_list_clear(void)
{
	buffer *op;

	while (numfree) {
		op = buffer_free_list[--numfree];
		PyMem_Free(op);
	}
}

static inline buffer*
alloc_buffer(void)
{
    buffer *buf;
	if (numfree) {
		buf = buffer_free_list[--numfree];
        DEBUG("use pooled buf %p\n", buf);
    }else{
        buf = (buffer *)PyMem_Malloc(sizeof(buffer));
        DEBUG("alloc buf %p\n", buf);
    }
    memset(buf, 0, sizeof(buffer));
    return buf;
}

static inline void
dealloc_buffer(buffer *buf)
{
	if (numfree < MAXFREELIST){
        DEBUG("back to buffer pool %p\n", buf);
		buffer_free_list[numfree++] = buf;
    }else{
	    PyMem_Free(buf);
    }
}

static inline int
hex2int(int i)
{
    i = toupper(i);
    i = i - '0';
    if(i > 9){ 
        i = i - 'A' + '9' + 1;
    }
    return i;
}

static inline int
urldecode(char *buf, int len)
{
    int c, c1;
    char *s0, *t;
    t = s0 = buf;
    while(len >0){
        c = *buf++;
        if(c == '%' && len > 2){
            c = *buf++;
            c1 = c;
            c = *buf++;
            c = hex2int(c1) * 16 + hex2int(c);
            len -= 2;
        }
        *t++ = c;
        len--;
    }
    *t = 0;
    return t - s0;
}

inline buffer *
new_buffer(size_t buf_size, size_t limit)
{
    buffer *buf;
    
    buf = alloc_buffer();

    buf->buf = PyMem_Malloc(sizeof(char) * buf_size);
    buf->buf_size = buf_size;
    if(limit){
        buf->limit = limit;
    }else{
        buf->limit = LIMIT_MAX;
    }
    return buf;
}

inline buffer_result
write2buf(buffer *buf, const char *c, size_t  l) {
    size_t newl;
    char *newbuf;
    buffer_result ret = WRITE_OK;
    newl = buf->len + l;
    
    
    if (newl >= buf->buf_size) {
        buf->buf_size *= 2;
        if(buf->buf_size <= newl) {
            buf->buf_size = (int)(newl + 1);
        }
        if(buf->buf_size > buf->limit){
            buf->buf_size = buf->limit + 1;
        }
        newbuf = (char*)PyMem_Realloc(buf->buf, buf->buf_size);
        if (!newbuf) {
            PyErr_SetString(PyExc_MemoryError,"out of memory");
            free(buf->buf);
            buf->buf = 0;
            buf->buf_size = buf->len = 0;
            return MEMORY_ERROR;
        }
        buf->buf = newbuf;
    }
    if(newl >= buf->buf_size){
        l = buf->buf_size - buf->len -1;
        ret = LIMIT_OVER;
    }
    memcpy(buf->buf + buf->len, c , l);
    buf->len += (int)l;
    return ret;
}

inline void
free_buffer(buffer *buf)
{
    PyMem_Free(buf->buf);
    dealloc_buffer(buf);
}

inline PyObject *
getPyString(buffer *buf)
{
    PyObject *o;
    o = PyString_FromStringAndSize(buf->buf, buf->len);
    free_buffer(buf);
    return o;
}

inline PyObject *
getPyStringAndDecode(buffer *buf)
{
    PyObject *o;
    int l = urldecode(buf->buf, buf->len);
    o = PyString_FromStringAndSize(buf->buf, l);
    free_buffer(buf);
    return o;
}


inline char *
getString(buffer *buf)
{
    buf->buf[buf->len] = '\0';
    return buf->buf;
}






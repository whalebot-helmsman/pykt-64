#ifndef UTIL_C
#define UTIL_C

#include "pykt.h"

inline int
urldecode(char *buf, int len);

inline void 
urlencode(char *str, size_t len, char **s, size_t *s_len);

#endif 

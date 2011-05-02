#ifndef TSV_PARSER_H
#define TSV_PARSER_H

#include "pykt.h" 


typedef void (*key_cb)(void *data, const char *buf, size_t length);
typedef void (*value_cb)(void *data, const char *buf, size_t length);

typedef struct {
	int cs;
    size_t key_start;
    size_t value_start;

    size_t nread;
    uint8_t error;

    key_cb on_key;
    value_cb on_value;

    void *user;
} tsv_ctx;

#endif


#ifndef HASH_CONSTANTS_POOL_H
#define HASH_CONSTANTS_POOL_H

#include "headers.h"
#include "h_memory.h"
#include "h_values.h"

typedef struct constants_pool_t {
    value_t* constants;
    size_t size;
    size_t capacity;
    value_t* current;
} constants_pool_t;

constants_pool_t* cp_init(size_t capacity);
size_t cp_write(constants_pool_t* pool, value_t constant);
void cp_free(constants_pool_t* pool);

#endif
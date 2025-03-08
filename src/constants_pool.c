#include <stddef.h>
#include <stdint.h>
#include "constants_pool.h"

constants_pool_t* cp_init(size_t capacity) {
    constants_pool_t* cp = (constants_pool_t*)malloc(sizeof(constants_pool_t));
    cp->constants = (value_t*)malloc(sizeof(value_t) * capacity);
    cp->size = 0;
    cp->capacity = capacity;
    cp->current = cp->constants;
    return cp;
}

size_t cp_write(constants_pool_t* pool, value_t constant) {
    if(pool->size >= pool->capacity) {
        pool->capacity *= 2;
        pool->constants = (value_t*)realloc(pool->constants, sizeof(value_t) * pool->capacity);
    }

    pool->constants[pool->size++] = constant;
    return pool->size - 1;
}

void cp_free(constants_pool_t* pool) {
    free(pool->constants);
    free(pool);
}
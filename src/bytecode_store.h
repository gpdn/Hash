#ifndef HASH_BYTECODE_STORE_H
#define HASH_BYTECODE_STORE_H

#include "headers.h"
#include "h_memory.h"
#include "h_values.h"
#include "constants_pool.h"

typedef enum opcode_t {
    OP_RETURN,
    OP_CONSTANT,
    OP_STOP,
    OP_START,
} opcode_t;

typedef struct bytecode_store_t {
    uint8_t* code;
    constants_pool_t* constants;
    size_t size;
    size_t capacity;
} bytecode_store_t;

bytecode_store_t* bs_init(size_t capacity);
void bs_write(bytecode_store_t* store, uint8_t instruction);
void bs_write_constant(bytecode_store_t* store, number_t constant);
void bs_free(bytecode_store_t* store);

#endif
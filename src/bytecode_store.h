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
    OP_NEGATE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_ERROR,
    OP_SHIFT_LEFT,
    OP_SHIFT_RIGHT,
    OP_BITWISE_AND,
    OP_BITWISE_OR,
    OP_BITWISE_XOR,
    OP_BITWISE_NOT,
    OP_EQUALITY,
    OP_NOT_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_GREATER_EQUAL,
    OP_LESS_EQUAL,
    OP_PRINT,
    OP_POP,
    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL,
    OP_ASSIGN,
    OP_PRE_INCREMENT,
    OP_PRE_DECREMENT
} opcode_t;

typedef struct bytecode_store_t {
    uint8_t* code;
    constants_pool_t* constants;
    size_t size;
    size_t capacity;
} bytecode_store_t;

bytecode_store_t* bs_init(size_t capacity);
void bs_write(bytecode_store_t* store, uint8_t instruction);
void bs_write_constant(bytecode_store_t* store, value_t constant);
void bs_free(bytecode_store_t* store);

#endif
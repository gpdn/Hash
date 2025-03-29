#ifndef HASH_BYTECODE_STORE_H
#define HASH_BYTECODE_STORE_H

#include "headers.h"
#include "h_memory.h"
#include "constants_pool.h"

typedef enum opcode_t {
    OP_NULL,
    OP_RETURN,
    OP_RETURN_VALUE,
    OP_CONSTANT,
    OP_PUSH_CONSTANT,
    OP_SET_PUSH_CONSTANT,
    OP_STOP,
    OP_STOP_VALUE,
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
    OP_PRE_INCREMENT,
    OP_PRE_DECREMENT,
    OP_POST_INCREMENT,
    OP_POST_DECREMENT,
    OP_SET_LOCAL,
    OP_GET_LOCAL,
    OP_GET_LOCAL_FUNCTION,
    OP_JUMP_IF_FALSE,
    OP_JUMP_IF_TRUE,
    OP_JUMP,
    OP_JUMP_PLACEHOLDER,
    OP_GOTO,
    OP_SET_LOCAL_ARRAY,
    OP_SET_LOCAL_INDEX,
    OP_GET_LOCAL_INDEX,
    OP_SET_LOCAL_INDEX_STRING,
    OP_GET_LOCAL_INDEX_STRING,
    OP_GENERATE_INTERVAL,
    OP_CALL,
    OP_DEFINE_LOCAL,
    OP_START_ARRAY_INITIALISATION,
    OP_GET_LOCAL_SIZE,
    OP_GET_LOCAL_SIZE_STRING,
    OP_SET_LOCAL_DATA,
    OP_GET_LOCAL_INDEX_COMPOUND,
    OP_SET_LOCAL_INDEX_COMPOUND,
    OP_GET_LOCAL_NATIVE,
    OP_CALL_NATIVE,
    OP_AND,
    OP_OR,
    OP_CONDITIONAL_EXPRESSION,
    OP_REWIND,
    OP_COPY
} opcode_t;

typedef struct bytecode_store_t {
    uint8_t* code;
    constants_pool_t* constants;
    size_t size;
    size_t capacity;
} bytecode_store_t;

bytecode_store_t* bs_init(size_t capacity);
void bs_write(bytecode_store_t* store, uint8_t instruction);
size_t bs_write_get(bytecode_store_t* store, uint8_t byte);
void bs_write_constant(bytecode_store_t* store, value_t constant);
void bs_free(bytecode_store_t* store);

#endif
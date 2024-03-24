#ifndef HASH_ICG_H
#define HASH_ICG_H

#include "h_token.h"
#include "bytecode_store.h"

typedef struct icg_t {
    token_t* tokens_list;
    bytecode_store_t* bytecode_store;
    token_t* current;
} icg_t;

icg_t* icg_init(token_t* tokens_list, size_t capacity);
bytecode_store_t* generate_bytecode(icg_t* icg);
void icg_free(icg_t* icg);

#endif
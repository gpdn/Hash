#ifndef HASH_ICG_H
#define HASH_ICG_H

#include "h_token.h"
#include "debug.h"
#include "bytecode_store.h"

typedef struct icg_t {
    token_t* tokens_list;
    size_t tokens_list_count;
    bytecode_store_t* bytecode_store;
    token_t* current;
    unsigned int errors_count;
} icg_t;

icg_t* icg_init(token_t* tokens_list, size_t tokens_list_count);
bytecode_store_t* icg_generate_bytecode(icg_t* icg);
void assert_token_type(icg_t* icg, token_type_t type);
void icg_free(icg_t* icg);

#endif
#include "h_icg.h"

icg_t* icg_init(token_t* tokens_list, size_t capacity) {
    icg_t* icg = (icg_t*)malloc(sizeof(icg_t));
    bytecode_store_t* bytecode_store = bs_init(capacity);
    icg->tokens_list = tokens_list;
}

void icg_free(icg_t* icg) {
    free(icg);
}

bytecode_store_t* generate_bytecode(icg_t* icg) {
    icg->current = icg->tokens_list;
    /* while(icg->current->type != H_TOKEN_EOF) {
        
    } */
}
#include "h_icg.h"

static void emit_error(icg_t* icg);
static void expression(icg_t* icg);
static void number(icg_t* icg);
static void grouping(icg_t* icg);

static void emit_error(icg_t* icg) {
    ++icg->errors_count;
    icg->current = &icg->tokens_list[icg->tokens_list_count - 1];
}

static void expression(icg_t* icg) {
    switch(icg->current->type) {
        case H_TOKEN_NUMBER_LITERAL:
            number(icg);
            break;
        case H_TOKEN_LEFT_PAR:
            grouping(icg);
            break;
        default:
            emit_error(icg);
            break;
    }
}

static void number(icg_t* icg) {
    double value = strtod(icg->current->start, NULL);
    if(value > UINT8_MAX) {emit_error(icg); return;}
    bs_write_constant(icg->bytecode_store, value);
    ++icg->current;
}

static void grouping(icg_t* icg) {
    ++icg->current;
    expression(icg);
    assert_token_type(icg, H_TOKEN_RIGHT_PAR);
}

icg_t* icg_init(token_t* tokens_list, size_t tokens_list_count) {
    icg_t* icg = (icg_t*)malloc(sizeof(icg_t));
    bytecode_store_t* bytecode_store = bs_init(tokens_list_count);
    icg->bytecode_store = bytecode_store;
    icg->tokens_list = tokens_list;
    return icg;
}

void icg_free(icg_t* icg) {
    free(icg);
}

void assert_token_type(icg_t* icg, token_type_t type) {
    if(icg->current->type == type) {
        ++icg->current;
        return;
    }
    ++icg->errors_count;
    
    icg->current = &icg->tokens_list[icg->tokens_list_count - 1];
}

bytecode_store_t* icg_generate_bytecode(icg_t* icg) {
    icg->current = icg->tokens_list;
    while(icg->current->type != H_TOKEN_EOF) {
        expression(icg);   
    }

    return icg->bytecode_store;
}
#ifndef HASH_ICG_H
#define HASH_ICG_H

#include "h_token.h"
#include "h_parser.h"
#include "debug.h"
#include "bytecode_store.h"
#include "h_string_t.h"

typedef struct icg_t {
    ast_node_t** ast_nodes_list;
    size_t ast_nodes_list_count;
    bytecode_store_t* bytecode_store;
    ast_node_t* current;
    ast_node_t** ast_nodes_list_it;
    unsigned int errors_count;
} icg_t;

icg_t* icg_init(ast_node_t** ast_nodes_list, size_t ast_nodes_list_count);
bytecode_store_t* icg_generate_bytecode(icg_t* icg);
void assert_node_type(icg_t* icg, ast_node_type_t type);
void icg_free(icg_t* icg);

#endif
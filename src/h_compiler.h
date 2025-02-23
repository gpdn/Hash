#ifndef H_COMPILER
#define H_COMPILER

#include <stdio.h>
#include "h_parser.h"
#include "h_file.h"
#include "h_ht_enums.h"
#include "h_ht_types.h"

typedef struct h_compiler_t {
    ast_node_t** ast_nodes_list;
    size_t ast_nodes_list_count;
    ast_node_t* current;
    h_string_t main_buffer;
    h_string_t c_buffer;
    h_string_t h_buffer;
    h_string_t *current_buffer;
    h_ht_types_t* types_table;
    h_ht_enums_t* enums_table;
} h_compiler_t;

h_compiler_t h_compiler_init(ast_node_t** ast_nodes_list, size_t ast_nodes_list_count, h_ht_types_t* types_table, h_ht_enums_t* enums_table);
void h_compiler_run(h_compiler_t* compiler);
void h_compiler_free(h_compiler_t* compiler);

#endif
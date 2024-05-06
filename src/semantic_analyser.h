#ifndef H_SEMANTIC_ANALYSER_H
#define H_SEMANTIC_ANALYSER_H

#include "h_parser.h"
#include "h_hash_table_t.h"
#include "h_locals_stack.h"

typedef struct semantic_analyser_t {
    ast_node_t** ast_nodes_list;
    ast_node_t** ast_nodes_list_it;
    size_t ast_nodes_list_count;
    h_hash_table_t* globals_table;
    h_locals_stack_t* locals;
    ast_node_t* current;
    unsigned int errors_count;
    size_t scope;
} semantic_analyser_t;

semantic_analyser_t* h_sa_init(ast_node_t** ast_nodes_list, size_t ast_nodes_list_count, h_hash_table_t* globals_table, h_locals_stack_t* locals);
void h_sa_free(semantic_analyser_t* analyser);
void h_sa_run(semantic_analyser_t* analyser);

#endif
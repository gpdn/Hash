#ifndef H_SEMANTIC_ANALYSER_H
#define H_SEMANTIC_ANALYSER_H

#include "h_parser.h"
#include "h_hash_table_t.h"
#include "h_locals_stack.h"
#include "h_ht_labels.h"
#include "h_ht_enums.h"
#include "h_ht_types.h"
#include "h_ht_fwd.h"
#include "h_internal_variables.h"

typedef struct returns_list_t {
    value_t* types;
    size_t size;
    size_t capacity;
} returns_list_t;

typedef struct semantic_analyser_t {
    ast_node_t** ast_nodes_list;
    ast_node_t** ast_nodes_list_it;
    ast_node_t** ast_nodes_post_checks;
    size_t ast_nodes_post_checks_size;
    size_t ast_nodes_post_checks_capacity;
    size_t ast_nodes_list_count;
    h_hash_table_t* globals_table;
    h_locals_stack_t* locals;
    h_locals_stack_t* initial_locals;
    h_ht_labels_t* labels_table;
    h_ht_enums_t* enums_table;
    h_ht_types_t* types_table;
    h_ht_fwd_t* fwd_table;
    ast_node_t* current;
    unsigned int errors_count;
    size_t scope;
    size_t loop_count;
    size_t return_count;
    returns_list_t returns_list;
} semantic_analyser_t;

semantic_analyser_t* h_sa_init(ast_node_t** ast_nodes_list, size_t ast_nodes_list_count, h_hash_table_t* globals_table, h_locals_stack_t* locals, h_ht_labels_t* labels_table, h_ht_enums_t* enums_table, h_ht_types_t* types_table, h_ht_fwd_t* fwd_table);
void h_sa_free(semantic_analyser_t* analyser);
void h_sa_run(semantic_analyser_t* analyser);

#endif
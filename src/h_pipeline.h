#ifndef HASH_PIPELINE_H
#define HASH_PIPELINE_H

#include "debug.h"
#include "h_args_flags.h"
#include "h_file.h"
#include "h_timer.h"
#include "h_ht_types.h"
#include "h_ht_fwd.h"
#include "bytecode_store.h"
#include "std/h_std.h"
#include "lexer.h"
#include "h_parser.h"
#include "h_internal_variables.h"
#include "h_counters_t.h"
#include "semantic_analyser.h"
#include "h_compiler.h"
#include "h_icg.h"
#include "virtual_machine.h"

typedef struct pipeline_t {
    clock_t timer;
    lexer_t* lexer;
    token_t* tokens_array;
    ast_node_t** ast;
    parser_t* parser;
    h_hash_table_t* globals_table;
    h_locals_stack_t* locals_stack;
    h_ht_labels_t* labels_table;
    h_ht_enums_t* enums_table;
    h_ht_types_t* types_table;
    h_ht_fwd_t* fwd_table;
    h_switch_tables_list_t* switch_tables_list;
    h_counters_t* counters;
    semantic_analyser_t* analyser;
    h_compiler_t compiler;
    icg_t* bytecode_generator;
    bytecode_store_t* store;
    virtual_machine_t* vm;
} pipeline_t;

int pipeline_start(const char* source_path, uint8_t flags, h_std_t* std);
void pipeline_free(pipeline_t* pipeline);

#endif
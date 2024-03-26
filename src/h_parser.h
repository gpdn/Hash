#ifndef HASH_PARSER_H
#define HASH_PARSER_H

#include "debug.h"
#include "h_token.h"

typedef enum ast_node_type_t{
    AST_NODE_BINARY,
    AST_NODE_LITERAL
} ast_node_type_t;

typedef struct ast_node_t {
    ast_node_type_t type;
    struct ast_node_t* left;
    struct ast_node_t* right;

} ast_node_t;

typedef struct parser_t {
    ast_node_t* root;
    token_t* tokens_list;
    size_t tokens_list_count;
    token_t* current;
    unsigned int errors_count;
} parser_t;

parser_t* parser_init(token_t* tokens_list);
ast_node_t* parser_generate_ast(parser_t* parser);
void parser_free(parser_t* parser);

#endif
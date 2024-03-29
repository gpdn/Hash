#ifndef HASH_PARSER_H
#define HASH_PARSER_H

#include "debug.h"
#include "h_token.h"

typedef enum ast_node_type_t{
    AST_NODE_BINARY,
    AST_NODE_LITERAL,
    AST_NODE_ERROR,
    AST_NODE_EOF,
    AST_NODE_GROUPING,
    AST_NODE_NUMBER_NEGATE
} ast_node_type_t;

typedef enum operator_precedence_t {
    OP_PREC_LOWEST,
    OP_PREC_ASSIGNMENT,
    OP_PREC_OR,
    OP_PREC_AND,
    OP_PREC_EQUALITY,
    OP_PREC_COMPARISON,
    OP_PREC_TERM,
    OP_PREC_FACTOR,
    OP_PREC_UNARY,
    OP_PREC_CALL,
    OP_PREC_HIGHEST
} operator_precedence_t;

typedef struct ast_node_t {
    ast_node_type_t type;
    struct ast_node_t* left;
    struct ast_node_t* right;
    token_t* operator;
} ast_node_t;

typedef struct parser_t {
    ast_node_t** ast_nodes_list;
    ast_node_t* current_node;
    size_t ast_list_capacity;
    size_t ast_list_size;
    token_t* tokens_list;
    size_t tokens_list_count;
    token_t* current;
    unsigned int errors_count;
} parser_t;

typedef ast_node_t* (nud_parse_function)(parser_t* parser, operator_precedence_t precedence);
typedef ast_node_t* (led_parse_function)(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);

typedef struct parse_rule_t {
    nud_parse_function* null_denotation;
    led_parse_function* left_denotation;
    operator_precedence_t precedence;
} parse_rule_t;

parser_t* parser_init(token_t* tokens_list, size_t tokens_list_size);
ast_node_t** parser_generate_ast(parser_t* parser);
void ast_print(ast_node_t* node, int indent);
void parser_free(parser_t* parser);

#endif
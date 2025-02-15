#ifndef HASH_PARSER_H
#define HASH_PARSER_H

#include <stdarg.h>
#include "debug.h"
#include "h_token.h"
#include "h_values.h"

typedef enum ast_node_type_t{
    AST_NODE_BINARY,
    AST_NODE_LITERAL,
    AST_NODE_IDENTIFIER,
    AST_NODE_IDENTIFIER_TYPE,
    AST_NODE_IDENTIFIER_FUNCTION,
    AST_NODE_IDENTIFIER_GLOBAL,
    AST_NODE_ERROR,
    AST_NODE_EOF,
    AST_NODE_GROUPING,
    AST_NODE_UNARY,
    AST_NODE_POST_UNARY,
    AST_NODE_ASSIGNMENT,
    AST_NODE_STATEMENT_PRINT,
    AST_NODE_STATEMENT_EXPRESSION,
    AST_NODE_STATEMENT_BLOCK,
    AST_NODE_DECLARATION_VARIABLE,
    AST_NODE_DECLARATION_VARIABLE_GLOBAL,
    AST_NODE_DECLARATION_VARIABLE_CONSTANT,
    AST_NODE_DECLARATION_LABEL,
    AST_NODE_STATEMENT_GOTO,
    AST_NODE_STATEMENT_IF,
    AST_NODE_STATEMENT_WHILE,
    AST_NODE_STATEMENT_DO_WHILE,
    AST_NODE_STATEMENT_ASSERTION,
    AST_NODE_STATEMENT_FOR,
    AST_NODE_STATEMENT_FOR_CONDITION,
    AST_NODE_DECLARATION_ENUM,
    AST_NODE_STATEMENT_BLOCK_ENUM,
    AST_NODE_ENUM_RESOLUTION,
    AST_NODE_INDEXING,
    AST_NODE_DECLARATION_VARIABLE_ARRAY,
    AST_NODE_ARRAY_INITIALISATION,
    AST_NODE_STATEMENT_REPEAT,
    AST_NODE_IDENTIFIER_TEMP,
    AST_NODE_STATEMENT_LOOP,
    AST_NODE_ASSIGNMENT_COMPOUND,
    AST_NODE_DECLARATION_FUNCTION,
    AST_NODE_FUNCTION_RETURN,
    AST_NODE_FUNCTION_CALL,
    AST_NODE_FUNCTION_ARGUMENTS,
    AST_NODE_FUNCTION_PARAMETERS,
    AST_NODE_STATEMENT_BREAK,
    AST_NODE_STATEMENT_SKIP,
    AST_NODE_STATEMENT_RETURN,
    AST_NODE_STATEMENT_RETURN_VALUE,
    AST_NODE_TO,
    AST_NODE_TEMP_CONTAINER,
    AST_NODE_DECLARATION_DATA,
    AST_NODE_DATA_INITIALISATION,
    AST_NODE_DOT,
    AST_NODE_IDENTIFIER_NATIVE,
    AST_NODE_NATIVE_CALL
} ast_node_type_t;

typedef enum operator_precedence_t {
    OP_PREC_LOWEST,
    OP_PREC_ASSIGNMENT,
    OP_PREC_OR,
    OP_PREC_AND,
    OP_PREC_BITWISE_OR,
    OP_PREC_BITWISE_XOR,
    OP_PREC_BITWISE_AND,
    OP_PREC_EQUALITY,
    OP_PREC_COMPARISON,
    OP_PREC_BITWISE_SHIFT,
    OP_PREC_TERM,
    OP_PREC_FACTOR,
    OP_PREC_UNARY,
    OP_PREC_CALL,
    OP_PREC_HIGHEST
} operator_precedence_t;

typedef struct ast_node_expression_t {
    struct ast_node_t* left;
    struct ast_node_t* right;
    struct ast_node_t* other;
} ast_node_expression_t;

typedef struct ast_node_statement_block_t {
    struct ast_node_t** declarations;
    size_t declarations_size;
    size_t declarations_capacity;
} ast_node_statement_block_t;

typedef struct ast_node_t {
    ast_node_type_t type;
    token_t* operator;
    value_t value;
    union {
        ast_node_expression_t expression;
        ast_node_statement_block_t block;
    };
} ast_node_t;

typedef struct parser_t {
    ast_node_t** ast_nodes_list;
    size_t ast_list_capacity;
    size_t ast_list_size;
    token_t* tokens_list;
    size_t tokens_list_count;
    token_t* current;
    unsigned int errors_count;
    ast_node_t* temp_one;
    ast_node_t* temp_two;
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
void disassemble_ast_node(ast_node_t* node, int indent);
void parser_free(parser_t* parser);

#endif
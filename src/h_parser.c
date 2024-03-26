#include "h_parser.h"

static inline ast_node_t* ast_node_create(ast_node_type_t type);
static void ast_node_free(ast_node_t* node);
static ast_node_t* parse_expression(parser_t* parser);
static ast_node_t* parse_binary_expression(parser_t* parser);
static inline void emit_error(parser_t* parser, const char* error_message);
static void assert_token_type(parser_t* parser, const char* error_message);

static void assert_token_type(parser_t* parser, token_type_t type, const char* error_message) {
    if(parser->current->type == type) {
        ++parser->current;
        return;
    }

    emit_error(parser, error_message);
}

static inline void emit_error(parser_t* parser, const char* error_message) {
    ++parser->errors_count;
    parser->current = parser->tokens_list + (parser->tokens_list_count - 1);
    DEBUG_LOG(error_message);
}

static inline ast_node_t* ast_node_create(ast_node_type_t type) {
    ast_node_t* node = (ast_node_t*)malloc(sizeof(ast_node_t));
    return node;
}

static void parse_number(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_LITERAL);
    return node;
} 

static ast_node_t* parse_expression(parser_t* parser) {
    /* switch (parser->current->type) {
        case H_TOKEN_NUMBER_LITERAL:
            parse_number(parser);
            break;
    } */
    return parse_binary_expression(parser);
}

static ast_node_t* parse_binary_expression(parser_t* parser) {
    ast_node_t* left = parse_expression(parser);
    return left;
}

static void ast_node_free(ast_node_t* node) {
    if(node->left) {
        ast_node_free(node->left);
    }

    if(node->right) {
        ast_node_free(node->right);
    }

    free(node);
}

parser_t* parser_init(token_t* tokens_array) {
    parser_t* parser = (parser_t*)malloc((sizeof(parser_t)));
    parser->root = NULL;
    parser->tokens_list = tokens_array;
    return parser;
}

void parser_free(parser_t* parser) {
    if(parser->root) {
        ast_node_free(parser->root);
    }
    free(parser);
}

ast_node_t* parser_generate_ast(parser_t* parser) {
    parser->current = parser->tokens_list;
    while(parser->current->type != H_TOKEN_EOF) {
        expression(parser->current);   
    }
}
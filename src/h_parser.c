#include "h_parser.h"

static inline ast_node_t* ast_node_create(ast_node_type_t type);
static void ast_node_free(ast_node_t* node);
static ast_node_t* parse_expression(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_binary_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);
static ast_node_t* parse_number(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_grouping(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_unary_negate(parser_t* parser, operator_precedence_t precedence);
static inline void emit_error(parser_t* parser, const char* error_message);
static void assert_token_type(parser_t* parser, token_type_t type, const char* error_message);
static void ast_nodes_array_push(parser_t* parser, ast_node_t* node); 

static parse_rule_t parse_table[] = {
    [H_TOKEN_NUMBER_LITERAL]            = {parse_number, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_LEFT_PAR]                  = {parse_grouping, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_PLUS]                      = {NULL, parse_binary_expression, OP_PREC_TERM},
    [H_TOKEN_MINUS]                     = {parse_unary_negate, parse_binary_expression, OP_PREC_UNARY},
    [H_TOKEN_STAR]                      = {NULL, parse_binary_expression, OP_PREC_FACTOR},
    [H_TOKEN_SLASH]                     = {NULL, parse_binary_expression, OP_PREC_FACTOR},
    [H_TOKEN_BITWISE_SHIFT_LEFT]        = {NULL, parse_binary_expression, OP_PREC_BITWISE_SHIFT},
    [H_TOKEN_BITWISE_SHIFT_RIGHT]       = {NULL, parse_binary_expression, OP_PREC_BITWISE_SHIFT},
    [H_TOKEN_BITWISE_AND]               = {NULL, parse_binary_expression, OP_PREC_BITWISE_AND},
    [H_TOKEN_BITWISE_OR]                = {NULL, parse_binary_expression, OP_PREC_BITWISE_OR},
};

static const char* parser_debug_colors[] = {COLOR_BLUE, COLOR_YELLOW, COLOR_GREEN, COLOR_RED};

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
    node->left = NULL;
    node->right = NULL;
    node->type = type;
    return node;
}

static ast_node_t* parse_number(parser_t* parser, operator_precedence_t precedence) {
    ast_node_t* node = ast_node_create(AST_NODE_LITERAL);
    node->operator = parser->current;
    ++parser->current;
    return node;
} 

static ast_node_t* parse_unary_negate(parser_t* parser, operator_precedence_t precedence) {
    ast_node_t* node = ast_node_create(AST_NODE_NUMBER_NEGATE);
    node->operator = parser->current;
    ++parser->current;
    node->left = parse_expression(parser, precedence);
    return node;
} 

static ast_node_t* parse_grouping(parser_t* parser, operator_precedence_t precedence) {
    ++parser->current;
    ast_node_t* node = parse_expression(parser, OP_PREC_LOWEST);
    assert_token_type(parser, H_TOKEN_RIGHT_PAR, "Expected )");
    return node;
} 

static ast_node_t* parse_expression(parser_t* parser, operator_precedence_t precedence) {
    
    parse_rule_t* nud = &parse_table[parser->current->type];

    if(nud->null_denotation == NULL) {
        emit_error(parser, "Expected expression.");
    }

    ast_node_t* left = nud->null_denotation(parser, nud->precedence); 

    parse_rule_t* led = &parse_table[parser->current->type];

    while(led->precedence > precedence) {
        left = led->left_denotation(parser, led->precedence, left);
        led = &parse_table[parser->current->type];
    }
    
    return left;
}

static ast_node_t* parse_binary_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left) {
    ast_node_t* node = ast_node_create(AST_NODE_BINARY);
    node->operator = parser->current;
    token_print(parser->current);
    ++parser->current;
    node->left = left;

    operator_precedence_t left_precedence = (parse_table + node->operator->type)->precedence;

    node->right = parse_expression(parser, left_precedence);

    return node;
}

static void ast_nodes_array_push(parser_t* parser, ast_node_t* node) {
    if(parser->ast_list_size >= parser->ast_list_capacity) {
        parser->ast_list_capacity *= 2; 
        parser->ast_nodes_list = (ast_node_t**)realloc(parser->ast_nodes_list, sizeof(ast_node_t*) * parser->ast_list_capacity);
    }

    parser->ast_nodes_list[parser->ast_list_size++] = node;
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

static void disassemble_ast_node(ast_node_t* node, int indent) {

    #define DEBUG_GET_NODE_COLOR() parser_debug_colors[indent % ((int)sizeof(parser_debug_colors) / sizeof(parser_debug_colors[0]))]
    #define DEBUG_NODE_COLOR(COLOR, ...) DEBUG_COLOR_SET(DEBUG_GET_NODE_COLOR()); DEBUG_LOG(__VA_ARGS__); DEBUG_COLOR_RESET()

    switch(node->type) {
        case AST_NODE_LITERAL:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_LITERAL %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_BINARY:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_BINARY %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_NUMBER_NEGATE:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_NUMBER_NEGATE %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_EOF:
            DEBUG_LOG("%d AST_NODE_EOF\n", indent);
            break;
        default: 
            DEBUG_LOG("Add node to ast disassembler. %d", node->type);
    }

}

parser_t* parser_init(token_t* tokens_array, size_t tokens_list_size) {
    parser_t* parser = (parser_t*)malloc((sizeof(parser_t)));
    parser->current_node = NULL;
    parser->ast_nodes_list = (ast_node_t**)malloc(sizeof(ast_node_t*) * tokens_list_size);
    parser->ast_list_capacity = tokens_list_size;
    parser->ast_list_size = 0;
    parser->tokens_list = tokens_array;
    return parser;
}

void parser_free(parser_t* parser) {
    for(size_t i = 0; i < parser->ast_list_size; ++i) {
        ast_node_free(parser->ast_nodes_list[i]);
    }
    free(parser->ast_nodes_list);
    free(parser);
}

void ast_print(ast_node_t* node, int indent) {
    disassemble_ast_node(node, indent);
    if(node->left) {
        ast_print(node->left, indent + 1);
    }
    if(node->right) {
        ast_print(node->right, indent + 1);
    }
}

ast_node_t** parser_generate_ast(parser_t* parser) {
    parser->current = parser->tokens_list;
    while(parser->current->type != H_TOKEN_EOF) {
        ast_node_t* node = parse_expression(parser, OP_PREC_LOWEST);
        ast_nodes_array_push(parser, node);
        if(node->type == AST_NODE_ERROR) {
            ++parser->errors_count; 
            return parser->ast_nodes_list;
        }   
    }
    ast_node_t* node = ast_node_create(AST_NODE_EOF);
    ast_nodes_array_push(parser, node);
    return parser->ast_nodes_list;
}
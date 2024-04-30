#include "h_parser.h"

static inline ast_node_t* ast_node_create(ast_node_type_t type);
static void ast_node_free(ast_node_t* node);
static ast_node_t* parse_expression(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_binary_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);
static ast_node_t* parse_number(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_string(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_grouping(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_unary_expression(parser_t* parser, operator_precedence_t precedence);
static inline ast_node_t* parse_expression_statement(parser_t* parser);
static inline ast_node_t* expression_statement(parser_t* parser);
static ast_node_t* parse_statement(parser_t* parser);
static ast_node_t* parse_declaration(parser_t* parser);
static ast_node_t* parse_print_statement(parser_t* parser);
static void emit_error(parser_t* parser, const char* error_message);
static void assert_token_type(parser_t* parser, token_type_t type, const char* error_message);
static void ast_nodes_array_push(parser_t* parser, ast_node_t* node); 

static parse_rule_t parse_table[] = {
    [H_TOKEN_NUMBER_LITERAL]            = {parse_number, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_STRING_LITERAL]            = {parse_string, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_LEFT_PAR]                  = {parse_grouping, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_PLUS]                      = {NULL, parse_binary_expression, OP_PREC_TERM},
    [H_TOKEN_MINUS]                     = {parse_unary_expression, parse_binary_expression, OP_PREC_UNARY},
    [H_TOKEN_STAR]                      = {NULL, parse_binary_expression, OP_PREC_FACTOR},
    [H_TOKEN_SLASH]                     = {NULL, parse_binary_expression, OP_PREC_FACTOR},
    [H_TOKEN_BITWISE_SHIFT_LEFT]        = {NULL, parse_binary_expression, OP_PREC_BITWISE_SHIFT},
    [H_TOKEN_BITWISE_SHIFT_RIGHT]       = {NULL, parse_binary_expression, OP_PREC_BITWISE_SHIFT},
    [H_TOKEN_BITWISE_AND]               = {NULL, parse_binary_expression, OP_PREC_BITWISE_AND},
    [H_TOKEN_BITWISE_OR]                = {NULL, parse_binary_expression, OP_PREC_BITWISE_OR},
    [H_TOKEN_BITWISE_XOR]               = {NULL, parse_binary_expression, OP_PREC_BITWISE_XOR},
    [H_TOKEN_BITWISE_NOT]               = {parse_unary_expression, NULL, OP_PREC_UNARY},
    [H_TOKEN_DOUBLE_EQUAL]              = {NULL, parse_binary_expression, OP_PREC_EQUALITY},
    [H_TOKEN_BANG_EQUAL]                = {NULL, parse_binary_expression, OP_PREC_EQUALITY},
    [H_TOKEN_GREATER]                   = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_GREATER_EQUAL]             = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_LESS]                      = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_LESS_EQUAL]                = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_LAST]                      = {NULL, NULL, OP_PREC_HIGHEST}
};

static const char* parser_debug_colors[] = {COLOR_BLUE, COLOR_YELLOW, COLOR_GREEN, COLOR_RED};

static void assert_token_type(parser_t* parser, token_type_t type, const char* error_message) {
    if(parser->current->type == type) {
        ++parser->current;
        return;
    }

    if(parser->errors_count == 0) emit_error(parser, error_message);
}

static void emit_error(parser_t* parser, const char* error_message) {
    DEBUG_COLOR_SET(COLOR_RED);
    ++parser->errors_count;
    fprintf(stderr, "\n\n[PARSER ERROR | Line: %lld] %.*s\n", parser->current->line, (int)parser->current->length, parser->current->start);
    
    token_t* temp = parser->current;
    token_t* temp_next = parser->current;

    size_t lines_count_before = temp->line - parser->tokens_list[0].line;
    size_t lines_count_after = parser->tokens_list[parser->tokens_list_count - 1].line - temp->line;

    if(lines_count_before > 0) {
        while(temp != parser->tokens_list && temp->line != temp->line - DEBUG_MIN(lines_count_before, 5)) --temp;
        while(temp_next != parser->tokens_list && temp_next->line == parser->current->line) --temp_next;
        ++temp_next;
        DEBUG_COLOR_SET(COLOR_GRAY);
        fprintf(stderr, "%.*s", (int)(temp_next->start - temp->start), temp->start);
    }

    DEBUG_COLOR_SET(COLOR_YELLOW);
    fprintf(stderr, "%.*s", (int)(parser->current->start - temp_next->start), temp_next->start);

    DEBUG_COLOR_SET(COLOR_RED);
    fprintf(stderr, "%.*s", (int)parser->current->length, parser->current->start);

    temp = parser->current;

    while(temp_next != (parser->tokens_list + parser->tokens_list_count) && temp_next->line == temp->line) ++temp_next;
    
    DEBUG_COLOR_SET(COLOR_YELLOW);
    fprintf(stderr, "%.*s", (int)(temp_next->start - (temp->start + temp->length)), temp->start + temp->length);
    
    temp = temp_next;
    while(temp_next != parser->tokens_list + parser->tokens_list_count && temp_next->line != temp->line + DEBUG_MIN(lines_count_after, 5)) {++temp_next;}
    DEBUG_COLOR_SET(COLOR_GRAY);
    fprintf(stderr, "%.*s", (int)(temp_next->start - temp->start), temp->start);

    fprintf(stderr, "\n");

    DEBUG_COLOR_SET(COLOR_RED);
    DEBUG_LOG(error_message);
    DEBUG_COLOR_RESET();
    parser->current = parser->tokens_list + (parser->tokens_list_count - 1);
}

static inline ast_node_t* ast_node_create(ast_node_type_t type) {
    ast_node_t* node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->left = NULL;
    node->right = NULL;
    node->type = type;
    return node;
}

static ast_node_t* parse_declaration(parser_t* parser) {
    switch(parser->current->type) {
        default: 
            return parse_statement(parser);
    }
}

static ast_node_t* parse_statement(parser_t* parser) {
    switch(parser->current->type) {
        case H_TOKEN_PRINT:
            return parse_print_statement(parser);
        default:
            return parse_expression_statement(parser);
    }
}

static ast_node_t* parse_print_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_PRINT);
    node->operator = parser->current;
    ++parser->current;
    node->left = expression_statement(parser);
    return node;
}

static inline ast_node_t* parse_expression_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_EXPRESSION); 
    node->left = parse_expression(parser, OP_PREC_LOWEST);
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected semicolon.");
    return node;
}

static inline ast_node_t* expression_statement(parser_t* parser) {
    ast_node_t* node = parse_expression(parser, OP_PREC_LOWEST);
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected semicolon.");
    return node;
}

static ast_node_t* parse_number(parser_t* parser, operator_precedence_t precedence) {
    ast_node_t* node = ast_node_create(AST_NODE_LITERAL);
    node->operator = parser->current;
    double value = strtod(node->operator->start, NULL);
    node->value = NUM_VALUE(value);
    ++parser->current;
    return node;
} 

static ast_node_t* parse_string(parser_t* parser, operator_precedence_t precedence) {
    ast_node_t* node = ast_node_create(AST_NODE_LITERAL);
    node->operator = parser->current;
    h_string_t* value = h_string_init(node->operator->start, node->operator->length);
    node->value = STR_VALUE(value);
    ++parser->current;
    return node;
} 

static ast_node_t* parse_unary_expression(parser_t* parser, operator_precedence_t precedence) {
    ast_node_t* node = ast_node_create(AST_NODE_UNARY);
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
        return ast_node_create(AST_NODE_ERROR);
    }

    ast_node_t* left = nud->null_denotation(parser, nud->precedence); 

    parse_rule_t* led = &parse_table[parser->current->type];

    while(led->precedence > precedence) {
        if(led->left_denotation == NULL) { 
            emit_error(parser, "Expected left denotation\n");
        }
        left = led->left_denotation(parser, led->precedence, left);
        led = &parse_table[parser->current->type];
    }
    
    return left;
}

static ast_node_t* parse_binary_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left) {
    ast_node_t* node = ast_node_create(AST_NODE_BINARY);
    node->operator = parser->current;
    ++parser->current;
    node->left = left;

    //operator_precedence_t left_precedence = (parse_table + node->operator->type)->precedence;

    node->right = parse_expression(parser, precedence);

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

void disassemble_ast_node(ast_node_t* node, int indent) {

    #define DEBUG_GET_NODE_COLOR() parser_debug_colors[indent % ((int)sizeof(parser_debug_colors) / sizeof(parser_debug_colors[0]))]
    #define DEBUG_NODE_COLOR(COLOR, ...) DEBUG_COLOR_SET(DEBUG_GET_NODE_COLOR()); DEBUG_LOG(__VA_ARGS__); DEBUG_COLOR_RESET()

    switch(node->type) {
        case AST_NODE_LITERAL:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_LITERAL %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_BINARY:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_BINARY %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_UNARY:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_UNARY %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_STATEMENT_PRINT:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_PRINT %.*s\n", indent, (int)node->operator->length, node->operator->start);
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
    parser->tokens_list_count = tokens_list_size;
    parser->errors_count = 0;
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
        ast_node_t* node = parse_declaration(parser);
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
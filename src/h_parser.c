#include "h_parser.h"

#define H_BLOCK_DECLARATIONS_CAPACITY 10
#define H_PARAMETERS_LIST_CAPACITY 5

static inline ast_node_t* ast_node_create(ast_node_type_t type);
static inline ast_node_t* ast_node_create_statement_block(ast_node_type_t type);
static inline ast_node_t* ast_node_create_temp(const char* string, size_t length);
static void ast_node_free(ast_node_t* node);
static ast_node_t* parse_expression(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_binary_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);
static ast_node_t* parse_ternary_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);
static ast_node_t* parse_to_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);
static ast_node_t* parse_assignment_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);
static ast_node_t* parse_number(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_string(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_char(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_grouping(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_identifier_type(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);
static ast_node_t* parse_identifier(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_identifier_function(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_identifier_global(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_unary_expression(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_unary_identifier_expression(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_post_unary_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);
static inline ast_node_t* parse_expression_statement(parser_t* parser);
static inline ast_node_t* parse_expression_statement_do_while(parser_t* parser);
static inline ast_node_t* parse_arrow_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);
static inline ast_node_t* parse_function_arguments_list_arrow(parser_t* parser, ast_node_t* first_arg);
static inline ast_node_t* parse_function_call_arrow(parser_t* parser, ast_node_t* left, ast_node_t* first_arg);
static ast_node_t* parse_variable_declaration(parser_t* parser, value_type_t type);
static ast_node_t* parse_variable_declaration_global(parser_t* parser);
static ast_node_t* parse_variable_declaration_enum(parser_t* parser);
static ast_node_t* parse_variable_declaration_label(parser_t* parser);
static ast_node_t* parse_variable_declaration_array(parser_t* parser);
static ast_node_t* parse_variable_declaration_type(parser_t* parser);
static ast_node_t* parse_function_declaration(parser_t* parser);
static ast_node_t* parse_data_declaration(parser_t* parser);
static ast_node_t* parse_statement(parser_t* parser);
static ast_node_t* parse_declaration(parser_t* parser);
static ast_node_t* parse_if_statement(parser_t* parser);
static ast_node_t* parse_while_statement(parser_t* parser);
static ast_node_t* parse_do_while_statement(parser_t* parser);
static ast_node_t* parse_print_statement(parser_t* parser);
static ast_node_t* parse_print_statement_do_while(parser_t* parser);
static ast_node_t* parse_goto_statement(parser_t* parser);
static ast_node_t* parse_assertion_statement(parser_t* parser);
static ast_node_t* parse_for_statement_condition(parser_t* parser);
static ast_node_t* parse_for_statement(parser_t* parser);
static ast_node_t* parse_break_statement(parser_t* parser);
static ast_node_t* parse_skip_statement(parser_t* parser);
static ast_node_t* parse_skip_statement_do_while(parser_t* parser);
static ast_node_t* parse_return_statement(parser_t* parser);
static ast_node_t* parse_repeat_statement(parser_t* parser);
static ast_node_t* parse_stop_statement(parser_t* parser);
static ast_node_t* parse_loop_statement(parser_t* parser);
static ast_node_t* parse_block_statement(parser_t* parser);
static ast_node_t* parse_block_statement_enum(parser_t* parser);
static ast_node_t* parse_block_expression(parser_t* parser);
static ast_node_t* parse_block_expression_do_while(parser_t* parser);
static ast_node_t* parse_block_expression_statement(parser_t* parser);
static ast_node_t* parse_block_expression_statement_do_while(parser_t* parser);
static ast_node_t* parse_enum_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);
static ast_node_t* parse_indexing(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);
static ast_node_t* parse_function_call(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);
static ast_node_t* parse_function_arguments_list(parser_t* parser);
static ast_node_t* parse_function_parameters_list(parser_t* parser);
static ast_node_t* parse_array_initialisation(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_data_initialisation(parser_t* parser, operator_precedence_t precedence);
static ast_node_t* parse_dot_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);
static ast_node_t* parse_compound_assignment_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left);
static void emit_error(parser_t* parser, const char* error_message);
static inline void assert_token_type(parser_t* parser, token_type_t type, const char* error_message);
static inline void assert_token_type_no_advance(parser_t* parser, token_type_t type, const char* error_message);
static void ast_nodes_array_push(parser_t* parser, ast_node_t* node);  
static value_type_t parser_get_value_type(parser_t* parser);
static value_t parser_get_value(parser_t* parser);

static parse_rule_t parse_table[] = {
    [H_TOKEN_EQUAL]                     = {NULL, parse_assignment_expression, OP_PREC_ASSIGNMENT},
    [H_TOKEN_NUMBER_LITERAL]            = {parse_number, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_STRING_LITERAL]            = {parse_string, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_CHAR_LITERAL]              = {parse_char, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_LEFT_PAR]                  = {parse_grouping, parse_function_call, OP_PREC_HIGHEST},
    [H_TOKEN_LEFT_SQUARE]               = {parse_array_initialisation, parse_indexing, OP_PREC_HIGHEST},
    [H_TOKEN_LEFT_CURLY]                = {parse_data_initialisation, NULL, OP_PREC_LOWEST},
    [H_TOKEN_IDENTIFIER]                = {parse_identifier, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_GLOB]                      = {parse_identifier_global, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_DOUBLE_COLON]              = {NULL, parse_enum_expression, OP_PREC_HIGHEST},
    [H_TOKEN_PLUS_PLUS]                 = {parse_unary_identifier_expression, parse_post_unary_expression, OP_PREC_HIGHEST},
    [H_TOKEN_MINUS_MINUS]               = {parse_unary_identifier_expression, NULL, OP_PREC_UNARY},
    [H_TOKEN_ARROW]                     = {NULL, parse_arrow_expression, OP_PREC_CALL},
    [H_TOKEN_PLUS]                      = {NULL, parse_binary_expression, OP_PREC_TERM},
    [H_TOKEN_MINUS]                     = {parse_unary_expression, parse_binary_expression, OP_PREC_UNARY},
    [H_TOKEN_STAR]                      = {NULL, parse_binary_expression, OP_PREC_FACTOR},
    [H_TOKEN_SLASH]                     = {NULL, parse_binary_expression, OP_PREC_FACTOR},
    [H_TOKEN_AND]                       = {NULL, parse_binary_expression, OP_PREC_AND},
    [H_TOKEN_OR]                        = {NULL, parse_binary_expression, OP_PREC_OR},
    [H_TOKEN_BITWISE_SHIFT_LEFT]        = {NULL, parse_binary_expression, OP_PREC_BITWISE_SHIFT},
    [H_TOKEN_BITWISE_SHIFT_RIGHT]       = {NULL, parse_binary_expression, OP_PREC_BITWISE_SHIFT},
    [H_TOKEN_BITWISE_AND]               = {NULL, parse_binary_expression, OP_PREC_BITWISE_AND},
    [H_TOKEN_BITWISE_OR]                = {NULL, parse_binary_expression, OP_PREC_BITWISE_OR},
    [H_TOKEN_BITWISE_XOR]               = {NULL, parse_binary_expression, OP_PREC_BITWISE_XOR},
    [H_TOKEN_BITWISE_NOT]               = {parse_unary_expression, NULL, OP_PREC_UNARY},
    [H_TOKEN_DOUBLE_EQUAL]              = {NULL, parse_binary_expression, OP_PREC_EQUALITY},
    [H_TOKEN_BANG_EQUAL]                = {NULL, parse_binary_expression, OP_PREC_EQUALITY},
    [H_TOKEN_PLUS_EQUAL]                = {NULL, parse_compound_assignment_expression, OP_PREC_EQUALITY},
    [H_TOKEN_MINUS_EQUAL]               = {NULL, parse_compound_assignment_expression, OP_PREC_EQUALITY},
    [H_TOKEN_STAR_EQUAL]                = {NULL, parse_compound_assignment_expression, OP_PREC_EQUALITY},
    [H_TOKEN_SLASH_EQUAL]               = {NULL, parse_compound_assignment_expression, OP_PREC_EQUALITY},
    [H_TOKEN_GREATER]                   = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_GREATER_EQUAL]             = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_LESS]                      = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_LESS_EQUAL]                = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_TO]                        = {NULL, parse_to_expression, OP_PREC_HIGHEST},
    [H_TOKEN_DOT]                       = {NULL, parse_dot_expression, OP_PREC_CALL},
    [H_TOKEN_QUESTION_MARK]             = {NULL, parse_ternary_expression, OP_PREC_TERNARY},
    [H_TOKEN_LAST]                      = {NULL, NULL, OP_PREC_HIGHEST}
};

static parse_rule_t parse_table_array_initialisation[] = {
    [H_TOKEN_EQUAL]                     = {NULL, parse_assignment_expression, OP_PREC_ASSIGNMENT},
    [H_TOKEN_NUMBER_LITERAL]            = {parse_number, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_STRING_LITERAL]            = {parse_string, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_CHAR_LITERAL]              = {parse_char, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_LEFT_PAR]                  = {parse_grouping, parse_function_call, OP_PREC_HIGHEST},
    [H_TOKEN_LEFT_SQUARE]               = {parse_array_initialisation, parse_indexing, OP_PREC_HIGHEST},
    [H_TOKEN_IDENTIFIER]                = {parse_identifier, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_GLOB]                      = {parse_identifier_global, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_DOUBLE_COLON]              = {NULL, parse_enum_expression, OP_PREC_HIGHEST},
    [H_TOKEN_PLUS_PLUS]                 = {parse_unary_identifier_expression, parse_post_unary_expression, OP_PREC_HIGHEST},
    [H_TOKEN_MINUS_MINUS]               = {parse_unary_identifier_expression, NULL, OP_PREC_UNARY},
    [H_TOKEN_ARROW]                     = {NULL, parse_arrow_expression, OP_PREC_CALL},
    [H_TOKEN_PLUS]                      = {NULL, parse_binary_expression, OP_PREC_TERM},
    [H_TOKEN_MINUS]                     = {parse_unary_expression, parse_binary_expression, OP_PREC_UNARY},
    [H_TOKEN_STAR]                      = {NULL, parse_binary_expression, OP_PREC_FACTOR},
    [H_TOKEN_SLASH]                     = {NULL, parse_binary_expression, OP_PREC_FACTOR},
    [H_TOKEN_AND]                       = {NULL, parse_binary_expression, OP_PREC_AND},
    [H_TOKEN_OR]                        = {NULL, parse_binary_expression, OP_PREC_OR},
    [H_TOKEN_BITWISE_SHIFT_LEFT]        = {NULL, parse_binary_expression, OP_PREC_BITWISE_SHIFT},
    [H_TOKEN_BITWISE_SHIFT_RIGHT]       = {NULL, parse_binary_expression, OP_PREC_BITWISE_SHIFT},
    [H_TOKEN_BITWISE_AND]               = {NULL, parse_binary_expression, OP_PREC_BITWISE_AND},
    [H_TOKEN_BITWISE_OR]                = {NULL, parse_binary_expression, OP_PREC_BITWISE_OR},
    [H_TOKEN_BITWISE_XOR]               = {NULL, parse_binary_expression, OP_PREC_BITWISE_XOR},
    [H_TOKEN_BITWISE_NOT]               = {parse_unary_expression, NULL, OP_PREC_UNARY},
    [H_TOKEN_DOUBLE_EQUAL]              = {NULL, parse_binary_expression, OP_PREC_EQUALITY},
    [H_TOKEN_BANG_EQUAL]                = {NULL, parse_binary_expression, OP_PREC_EQUALITY},
    [H_TOKEN_PLUS_EQUAL]                = {NULL, parse_compound_assignment_expression, OP_PREC_EQUALITY},
    [H_TOKEN_MINUS_EQUAL]               = {NULL, parse_compound_assignment_expression, OP_PREC_EQUALITY},
    [H_TOKEN_STAR_EQUAL]                = {NULL, parse_compound_assignment_expression, OP_PREC_EQUALITY},
    [H_TOKEN_SLASH_EQUAL]               = {NULL, parse_compound_assignment_expression, OP_PREC_EQUALITY},
    [H_TOKEN_GREATER]                   = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_GREATER_EQUAL]             = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_LESS]                      = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_LESS_EQUAL]                = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_TO]                        = {NULL, parse_to_expression, OP_PREC_HIGHEST},
    [H_TOKEN_DOT]                       = {NULL, parse_dot_expression, OP_PREC_CALL},
    [H_TOKEN_QUESTION_MARK]             = {NULL, parse_ternary_expression, OP_PREC_TERNARY},
    [H_TOKEN_LAST]                      = {NULL, NULL, OP_PREC_HIGHEST}
};

static parse_rule_t parse_table_function[] = {
    [H_TOKEN_EQUAL]                     = {NULL, parse_assignment_expression, OP_PREC_ASSIGNMENT},
    [H_TOKEN_NUMBER_LITERAL]            = {parse_number, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_STRING_LITERAL]            = {parse_string, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_CHAR_LITERAL]              = {parse_char, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_LEFT_PAR]                  = {parse_grouping, parse_function_call, OP_PREC_HIGHEST},
    [H_TOKEN_LEFT_SQUARE]               = {parse_array_initialisation, parse_indexing, OP_PREC_HIGHEST},
    [H_TOKEN_LEFT_CURLY]                = {parse_data_initialisation, NULL, OP_PREC_LOWEST},
    [H_TOKEN_IDENTIFIER]                = {parse_identifier, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_GLOB]                      = {parse_identifier_global, NULL, OP_PREC_HIGHEST},
    [H_TOKEN_DOUBLE_COLON]              = {NULL, parse_enum_expression, OP_PREC_HIGHEST},
    [H_TOKEN_PLUS_PLUS]                 = {parse_unary_identifier_expression, parse_post_unary_expression, OP_PREC_HIGHEST},
    [H_TOKEN_MINUS_MINUS]               = {parse_unary_identifier_expression, NULL, OP_PREC_UNARY},
    [H_TOKEN_ARROW]                     = {NULL, parse_arrow_expression, OP_PREC_CALL},
    [H_TOKEN_PLUS]                      = {NULL, parse_binary_expression, OP_PREC_TERM},
    [H_TOKEN_MINUS]                     = {parse_unary_expression, parse_binary_expression, OP_PREC_UNARY},
    [H_TOKEN_STAR]                      = {NULL, parse_binary_expression, OP_PREC_FACTOR},
    [H_TOKEN_SLASH]                     = {NULL, parse_binary_expression, OP_PREC_FACTOR},
    [H_TOKEN_AND]                       = {NULL, parse_binary_expression, OP_PREC_AND},
    [H_TOKEN_OR]                        = {NULL, parse_binary_expression, OP_PREC_OR},
    [H_TOKEN_BITWISE_SHIFT_LEFT]        = {NULL, parse_binary_expression, OP_PREC_BITWISE_SHIFT},
    [H_TOKEN_BITWISE_SHIFT_RIGHT]       = {NULL, parse_binary_expression, OP_PREC_BITWISE_SHIFT},
    [H_TOKEN_BITWISE_AND]               = {NULL, parse_binary_expression, OP_PREC_BITWISE_AND},
    [H_TOKEN_BITWISE_OR]                = {NULL, parse_binary_expression, OP_PREC_BITWISE_OR},
    [H_TOKEN_BITWISE_XOR]               = {NULL, parse_binary_expression, OP_PREC_BITWISE_XOR},
    [H_TOKEN_BITWISE_NOT]               = {parse_unary_expression, NULL, OP_PREC_UNARY},
    [H_TOKEN_DOUBLE_EQUAL]              = {NULL, parse_binary_expression, OP_PREC_EQUALITY},
    [H_TOKEN_BANG_EQUAL]                = {NULL, parse_binary_expression, OP_PREC_EQUALITY},
    [H_TOKEN_PLUS_EQUAL]                = {NULL, parse_compound_assignment_expression, OP_PREC_EQUALITY},
    [H_TOKEN_MINUS_EQUAL]               = {NULL, parse_compound_assignment_expression, OP_PREC_EQUALITY},
    [H_TOKEN_STAR_EQUAL]                = {NULL, parse_compound_assignment_expression, OP_PREC_EQUALITY},
    [H_TOKEN_SLASH_EQUAL]               = {NULL, parse_compound_assignment_expression, OP_PREC_EQUALITY},
    [H_TOKEN_GREATER]                   = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_GREATER_EQUAL]             = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_LESS]                      = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_LESS_EQUAL]                = {NULL, parse_binary_expression, OP_PREC_COMPARISON},
    [H_TOKEN_TO]                        = {NULL, parse_to_expression, OP_PREC_HIGHEST},
    [H_TOKEN_DOT]                       = {NULL, parse_dot_expression, OP_PREC_CALL},
    [H_TOKEN_QUESTION_MARK]             = {NULL, parse_ternary_expression, OP_PREC_TERNARY},
    [H_TOKEN_LAST]                      = {NULL, NULL, OP_PREC_HIGHEST}
};

static parse_rule_t* current_parse_table = parse_table;

static const char* parser_debug_colors[] = {COLOR_BLUE, COLOR_YELLOW, COLOR_GREEN, COLOR_RED};

static inline void assert_token_type(parser_t* parser, token_type_t type, const char* error_message) {
    if(parser->current->type == type) {
        ++parser->current;
        return;
    }

    if(parser->errors_count == 0) emit_error(parser, error_message);
}

static inline void assert_token_type_no_advance(parser_t* parser, token_type_t type, const char* error_message) {
    if(parser->current->type == type) {
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
    node->expression.left = NULL;
    node->expression.right = NULL;
    node->expression.other = NULL;
    node->type = type;
    return node;
}

static inline ast_node_t* ast_node_create_statement_block(ast_node_type_t type) {
    ast_node_t* node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = type;
    node->block.declarations = NULL;
    node->block.declarations_size = 0;
    node->block.declarations_capacity = 0;
    return node;
}

static inline ast_node_t* ast_node_create_temp(const char* string, size_t length) {
    ast_node_t* node = (ast_node_t*)malloc(sizeof(ast_node_t));
    node->type = AST_NODE_IDENTIFIER_TEMP;
    node->expression.left = NULL;
    node->expression.right = NULL;
    node->expression.other = NULL;
    h_string_t* name = h_string_init_hash(string, length);
    node->operator = NULL;
    node->value = STR_VALUE(name);
    return node;
}

static ast_node_t* parse_declaration(parser_t* parser) {
    switch(parser->current->type) {
        case H_TOKEN_LABEL:
            return parse_variable_declaration_label(parser);
        case H_TOKEN_GLOB:
            return parse_variable_declaration_global(parser);    
        case H_TOKEN_ENUM:
            return parse_variable_declaration_enum(parser);
        case H_TOKEN_NUM:
            return parse_variable_declaration(parser, H_VALUE_NUMBER);
            break;
        case H_TOKEN_STR:
            return parse_variable_declaration(parser, H_VALUE_STRING);
            break;
        case H_TOKEN_CHAR:
            return parse_variable_declaration(parser, H_VALUE_CHAR);
            break;
        case H_TOKEN_ARR:
            return parse_variable_declaration_array(parser);
            break;
        case H_TOKEN_FUNCTION:
            return parse_function_declaration(parser);
            break;
        case H_TOKEN_DATA:
            return parse_data_declaration(parser);
            break;
        case H_TOKEN_TYPE:
            return parse_variable_declaration_type(parser);
            break;
        default: 
            return parse_statement(parser);
    }
}

static value_type_t parser_get_value_type(parser_t* parser) {
    switch(parser->current->type) {
        case H_TOKEN_NUM:
            return H_VALUE_NUMBER;
            break;
        case H_TOKEN_STR:
            return H_VALUE_STRING;
            break;
        case H_TOKEN_FUNCTION:
            return H_VALUE_FUNCTION;
            break;
        case H_TOKEN_DATA:
            return H_VALUE_DATA;
            break;
        case H_TOKEN_IDENTIFIER:
            return H_VALUE_TYPE;
            break;
        case H_TOKEN_ARR:
            return H_VALUE_ARRAY;
            break;
        default: 
            emit_error(parser, "Undefined type.");
            return H_VALUE_NULL;
    }
}

static value_t parser_get_value(parser_t* parser) {
    switch(parser->current->type) {
        case H_TOKEN_NUM:
            return NUM_VALUE(0);
            break;
        case H_TOKEN_STR:
            return STR_VALUE(NULL);
            break;
        case H_TOKEN_FUNCTION:
            return VALUE_FUNCTION(NULL);
            break;
        case H_TOKEN_DATA:
            return VALUE_DATA(NULL);
            break;
        case H_TOKEN_IDENTIFIER:
            h_string_t* type_name = h_string_init_hash(parser->current->start, parser->current->length);
            return (value_t){.type = H_VALUE_TYPE, .string = type_name};
            break;
        case H_TOKEN_ARR:
            return VALUE_ARRAY(NULL);
            break;
        default: 
            emit_error(parser, "Undefined type.");
            return NULL_VALUE();
    }
}

static ast_node_t* parse_block_expression_statement(parser_t* parser) {
    switch(parser->current->type) {
        case H_TOKEN_PRINT:
            return parse_print_statement(parser);
        case H_TOKEN_GOTO:
            return parse_goto_statement(parser);
        case H_TOKEN_BREAK:
            return parse_break_statement(parser);
        case H_TOKEN_SKIP:
            return parse_skip_statement(parser);
        case H_TOKEN_RETURN:
            return parse_return_statement(parser);
        case H_TOKEN_STOP:
            return parse_stop_statement(parser);
        default:
            return parse_expression_statement(parser);
    }
}

static ast_node_t* parse_block_expression_statement_do_while(parser_t* parser) {
    switch(parser->current->type) {
        case H_TOKEN_PRINT:
            return parse_print_statement_do_while(parser);
        case H_TOKEN_SKIP:
            return parse_skip_statement_do_while(parser);
        default:
            return parse_expression_statement_do_while(parser);
    }
}

static ast_node_t* parse_statement(parser_t* parser) {
    switch(parser->current->type) {
        case H_TOKEN_PRINT:
            return parse_print_statement(parser);
        case H_TOKEN_IF:
            return parse_if_statement(parser);
        case H_TOKEN_WHILE:
            return parse_while_statement(parser);
        case H_TOKEN_DO:
            return parse_do_while_statement(parser);
        case H_TOKEN_GOTO:
            return parse_goto_statement(parser);
        case H_TOKEN_FOR:
            return parse_for_statement(parser);
        case H_TOKEN_BREAK:
            return parse_break_statement(parser);
        case H_TOKEN_SKIP:
            return parse_skip_statement(parser);
        case H_TOKEN_RETURN:
            return parse_return_statement(parser);
        case H_TOKEN_REPEAT:
            return parse_repeat_statement(parser);
        case H_TOKEN_DOUBLE_BANG:
            return parse_assertion_statement(parser);
        case H_TOKEN_LOOP:
            return parse_loop_statement(parser);
        case H_TOKEN_LEFT_CURLY:
            return parse_block_statement(parser);
        case H_TOKEN_STOP:
            return parse_stop_statement(parser);
        default:
            return parse_expression_statement(parser);
    }
}

static ast_node_t* parse_variable_declaration(parser_t* parser, value_type_t type) {
    ast_node_t* node = ast_node_create(AST_NODE_DECLARATION_VARIABLE);
    node->operator = parser->current;
    node->value.type = type;
    ++parser->current;
    node->expression.left = parse_identifier(parser, OP_PREC_HIGHEST);
    assert_token_type(parser, H_TOKEN_EQUAL, "Expected =");
    node->expression.right = parse_expression(parser, OP_PREC_OR);
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected ;");
    return node;
}

static ast_node_t* parse_variable_declaration_global(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_DECLARATION_VARIABLE_GLOBAL);
    node->operator = parser->current;
    ++parser->current;
    node->value.type = parser_get_value_type(parser);
    ++parser->current;
    node->expression.left = parse_identifier(parser, OP_PREC_HIGHEST);
    assert_token_type(parser, H_TOKEN_EQUAL, "Expected =");
    node->expression.right = parse_expression(parser, OP_PREC_OR);
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected ;");
    return node;
}

static ast_node_t* parse_variable_declaration_array(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_DECLARATION_VARIABLE_ARRAY);
    node->operator = parser->current;
    ++parser->current;
    assert_token_type(parser, H_TOKEN_LESS, "Expected < after arr keyword.");
    node->value.type = parser_get_value_type(parser);
    ++parser->current;
    assert_token_type(parser, H_TOKEN_GREATER, "Expected > after type in arr keyword.");
    node->expression.left = parse_identifier(parser, OP_PREC_HIGHEST);
    assert_token_type(parser, H_TOKEN_EQUAL, "Expected =");
    node->expression.right = parse_expression(parser, OP_PREC_OR);
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected ;");
    return node;
}

static ast_node_t* parse_variable_declaration_type(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_DECLARATION_VARIABLE_TYPE);
    node->operator = parser->current;
    ++parser->current;
    h_string_t* value = h_string_init_hash(parser->current->start, parser->current->length);
    node->value = STR_VALUE(value);
    ++parser->current;
    h_generic_arguments_list_t* generics_arguments = NULL;
    if(parser->current->type == H_TOKEN_LESS) {
        ++parser->current;
        if(parser->current->type == H_TOKEN_GREATER) {
            emit_error(parser, "Expected generic parameters list in data declaration. Empty parameters list not allowed for generics.");
            return ast_node_create(AST_NODE_ERROR);
        }
        generics_arguments = h_generic_arguments_list_init(); 
        do {
            value_t value = parser_get_value(parser);
            h_generic_arguments_list_add(generics_arguments, value);
            ++parser->current;
        }
        while(parser->current->type == H_TOKEN_COMMA && ++parser->current);
        assert_token_type(parser, H_TOKEN_GREATER, "Expected > after generic parameters list in data declaration.");
        //h_generic_arguments_list_print(generics_arguments);
    }
    node->expression.other = generics_arguments;
    node->expression.left = parse_identifier(parser, OP_PREC_HIGHEST);
    assert_token_type(parser, H_TOKEN_EQUAL, "Expected =");
    node->expression.right = parse_expression(parser, OP_PREC_OR);
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected ;");
    node->value.type = H_VALUE_TYPE;
    return node;
}

static ast_node_t* parse_function_declaration(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_DECLARATION_FUNCTION);
    node->operator = parser->current;
    ++parser->current;
    h_function_t* function = h_function_init();
    assert_token_type(parser, H_TOKEN_LEFT_SQUARE, "Expected [ after fn keyword.");
    if(parser->current->type != H_TOKEN_RIGHT_SQUARE) {
        value_t return_type = parser_get_value(parser);
        function->return_type[0] = return_type;
        ++parser->current;
    }
    assert_token_type(parser, H_TOKEN_RIGHT_SQUARE, "Expected ] after fn return type.");
    assert_token_type_no_advance(parser, H_TOKEN_IDENTIFIER, "Expected function name after return types.");
    node->expression.left = parse_identifier_function(parser, OP_PREC_HIGHEST);
    function->name = node->expression.left->value.string;
    assert_token_type(parser, H_TOKEN_LEFT_PAR, "Expected ( after function name.");
    if(parser->current->type != H_TOKEN_RIGHT_PAR) {
        do {
            value_t type = parser_get_value(parser);
            ++parser->current;
            assert_token_type_no_advance(parser, H_TOKEN_IDENTIFIER, "Expected parameter name.");
            h_string_t* parameter_name = h_string_init_hash(parser->current->start, parser->current->length);
            ++parser->current;
            h_function_parameter_add(function, parameter_name, type);
        }
        while(parser->current->type == H_TOKEN_COMMA && ++parser->current);
    }
    assert_token_type(parser, H_TOKEN_RIGHT_PAR, "Expected ) after function parameters.");
    assert_token_type_no_advance(parser, H_TOKEN_LEFT_CURLY, "Expected {");
    current_parse_table = parse_table_function;
    node->expression.right = parse_block_statement(parser);
    current_parse_table = parse_table;
    node->value = VALUE_FUNCTION(function);
    return node;
}

static ast_node_t* parse_data_declaration(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_DECLARATION_DATA);
    node->operator = parser->current;
    ++parser->current;
    h_generic_parameters_list_t* generics = NULL;
    if(parser->current->type == H_TOKEN_LESS) {
        ++parser->current;
        if(parser->current->type == H_TOKEN_GREATER) {
            emit_error(parser, "Expected generic parameters list in data declaration. Empty parameters list not allowed for generics.");
            return ast_node_create(AST_NODE_ERROR);
        }
        generics = h_generic_parameters_list_init(); 
        do {
            assert_token_type_no_advance(parser, H_TOKEN_IDENTIFIER, "Expected generic name.");
            if(parser->current->length > 1) {
                emit_error(parser, "Expected character for generic parameter in data declaration.");
                return ast_node_create(AST_NODE_ERROR);
            }
            char c = *parser->current->start;
            h_generic_parameters_list_add(generics, c);
            ++parser->current;
        }
        while(parser->current->type == H_TOKEN_COMMA && ++parser->current);
        assert_token_type(parser, H_TOKEN_GREATER, "Expected > after generic parameters list in data declaration.");
        h_generic_parameters_list_print(generics);
    }
    assert_token_type_no_advance(parser, H_TOKEN_IDENTIFIER, "Expected name after data definition.");
    node->expression.left = parse_identifier(parser, OP_PREC_HIGHEST);
    h_struct_t* data = h_struct_init(node->expression.left->value.string);
    data->generics = generics;
    assert_token_type(parser, H_TOKEN_COLON, "Expected : after data name.");
    if(parser->current->type != H_TOKEN_COLON) {
        do {
            value_type_t type = parser_get_value_type(parser);
            if(type == H_VALUE_TYPE) {
                h_string_t* field_type = h_string_init_hash(parser->current->start, parser->current->length);
                ++parser->current;
                assert_token_type_no_advance(parser, H_TOKEN_IDENTIFIER, "Expected parameter name.");
                h_string_t* field_name = h_string_init_hash(parser->current->start, parser->current->length);
                ++parser->current;
                h_struct_field_add(data, field_name, (value_t){.type = type, .string = field_type});
                continue;
            }
            ++parser->current;
            assert_token_type_no_advance(parser, H_TOKEN_IDENTIFIER, "Expected parameter name.");
            h_string_t* field_name = h_string_init_hash(parser->current->start, parser->current->length);
            ++parser->current;
            h_struct_field_add(data, field_name, (value_t){.type = type});
        }
        while(parser->current->type == H_TOKEN_SEMICOLON && ++parser->current && parser->current->type != H_TOKEN_COLON);
    }
    assert_token_type(parser, H_TOKEN_COLON, "Expected : after data definition.");
    node->value = VALUE_DATA(data);
    return node;
}

static ast_node_t* parse_variable_declaration_enum(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_DECLARATION_ENUM);
    ++parser->current;
    node->operator = parser->current;
    node->expression.left = parse_identifier(parser, OP_PREC_HIGHEST);
    assert_token_type_no_advance(parser, H_TOKEN_COLON, "Expected : for enum declaration");
    node->expression.right = parse_block_statement_enum(parser);
    //assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected ;");
    return node;
}

static ast_node_t* parse_block_statement_enum(parser_t* parser) {
    ast_node_t* node = ast_node_create_statement_block(AST_NODE_STATEMENT_BLOCK_ENUM);
    node->operator = parser->current;
    ++parser->current;
    node->block.declarations_capacity = H_BLOCK_DECLARATIONS_CAPACITY;
    node->block.declarations = (ast_node_t**)malloc(sizeof(ast_node_t*) * node->block.declarations_capacity);
    if(parser->current->type == H_TOKEN_COLON) {
        emit_error(parser, "Empty enum not allowed");
        return ast_node_create(AST_NODE_ERROR);
    }
    do {
        if(node->block.declarations_size >= node->block.declarations_capacity) {
            node->block.declarations_capacity *= 2;
            node->block.declarations = realloc(node->block.declarations, sizeof(ast_node_t*) * node->block.declarations_capacity);
        }
        node->block.declarations[node->block.declarations_size++] = parse_identifier(parser, OP_PREC_HIGHEST);
    } while(parser->current->type == H_TOKEN_COMMA && (++parser->current)->type != H_TOKEN_COLON);
    assert_token_type(parser, H_TOKEN_COLON, "Expected :.");
    return node;
}

static ast_node_t* parse_variable_declaration_label(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_DECLARATION_LABEL);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = parse_identifier(parser, OP_PREC_HIGHEST);
    assert_token_type(parser, H_TOKEN_COLON, "Expected :");
    return node;
}

static ast_node_t* parse_print_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_PRINT);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = parse_expression(parser, OP_PREC_LOWEST);
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected semicolon.");
    return node;
}

static ast_node_t* parse_print_statement_do_while(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_PRINT);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = parse_expression(parser, OP_PREC_LOWEST);
    return node;
}

static ast_node_t* parse_block_expression(parser_t* parser) {
    ast_node_t* node = ast_node_create_statement_block(AST_NODE_STATEMENT_BLOCK);
    node->operator = parser->current;
    node->block.declarations_capacity = 1;
    node->block.declarations = (ast_node_t**)malloc(sizeof(ast_node_t*) * node->block.declarations_capacity);
    node->block.declarations[node->block.declarations_size++] = parse_block_expression_statement(parser);
    return node;
}

static ast_node_t* parse_block_expression_do_while(parser_t* parser) {
    ast_node_t* node = ast_node_create_statement_block(AST_NODE_STATEMENT_BLOCK);
    node->operator = parser->current;
    node->block.declarations_capacity = 1;
    node->block.declarations = (ast_node_t**)malloc(sizeof(ast_node_t*) * node->block.declarations_capacity);
    node->block.declarations[node->block.declarations_size++] = parse_block_expression_statement_do_while(parser);
    return node;
}

static ast_node_t* parse_if_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_IF);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = parse_expression(parser, OP_PREC_LOWEST);
    if(parser->current->type == H_TOKEN_LEFT_CURLY) {
        node->expression.right = parse_block_statement(parser);
        if(parser->current->type == H_TOKEN_ELSE) {
            ++parser->current;
            assert_token_type_no_advance(parser, H_TOKEN_LEFT_CURLY, "Expected {.");
            node->expression.other = parse_block_statement(parser);
        }
        return node;
    }
    node->expression.right = parse_block_expression(parser);
    return node;
}

static ast_node_t* parse_while_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_WHILE);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = parse_expression(parser, OP_PREC_LOWEST);
    if(parser->current->type == H_TOKEN_LEFT_CURLY) {
        node->expression.right = parse_block_statement(parser);
        return node;
    }
    node->expression.right = parse_block_expression(parser);
    return node;
}

static ast_node_t* parse_do_while_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_DO_WHILE);
    node->operator = parser->current;
    ++parser->current;
    if(parser->current->type == H_TOKEN_LEFT_CURLY) {
        node->expression.left = parse_block_statement(parser);
        assert_token_type(parser, H_TOKEN_WHILE, "Expected while after do statement.");
        node->expression.right = parse_expression(parser, OP_PREC_LOWEST);
        assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected ; after do while statement");
        return node;
    }
    node->expression.left = parse_block_expression_do_while(parser);
    assert_token_type(parser, H_TOKEN_WHILE, "Expected while after do statement.");
    node->expression.right = parse_expression(parser, OP_PREC_LOWEST);
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected ; after do while statement");
    return node;
}

static ast_node_t* parse_goto_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_GOTO);
    node->operator = parser->current;
    ++parser->current;
    assert_token_type_no_advance(parser, H_TOKEN_IDENTIFIER, "Expected identifier after goto.");
    node->expression.left = parse_identifier(parser, OP_PREC_HIGHEST);
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected :");
    return node;
}

static ast_node_t* parse_assertion_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_ASSERTION);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = parse_expression(parser, OP_PREC_LOWEST);
    node->expression.right = parse_print_statement(parser);
    return node;
}

static ast_node_t* parse_for_statement_condition(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_FOR_CONDITION);
    node->operator = parser->current;
    ++parser->current;
    if(parser->current->type == H_TOKEN_NUM) node->expression.left = parse_variable_declaration(parser, H_VALUE_NUMBER);
    node->expression.other = parse_expression(parser, OP_PREC_LOWEST);
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected semicolon after for termination condition.");
    node->expression.right = parse_expression(parser, OP_PREC_LOWEST);
    return node;
}

static ast_node_t* parse_for_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_FOR);
    node->operator = parser->current;
    ++parser->current;
    assert_token_type_no_advance(parser, H_TOKEN_LEFT_PAR, "Expected ( after for statement.");
    node->expression.left = parse_for_statement_condition(parser);
    assert_token_type(parser, H_TOKEN_RIGHT_PAR, "Expected ) after for statement.");
    if(parser->current->type == H_TOKEN_LEFT_CURLY) {
        node->expression.right = parse_block_statement(parser);
        return node;
    }
    node->expression.right = parse_block_expression(parser);
    return node;
}

static ast_node_t* parse_break_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_BREAK);
    node->operator = parser->current;
    ++parser->current;
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected semicolon after break statement");
    return node;
}

static ast_node_t* parse_skip_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_SKIP);
    node->operator = parser->current;
    ++parser->current;
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected semicolon after break statement");
    return node;
}

static ast_node_t* parse_skip_statement_do_while(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_SKIP);
    node->operator = parser->current;
    ++parser->current;
    return node;
}

static ast_node_t* parse_return_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_RETURN);
    node->operator = parser->current;
    ++parser->current;
    if(parser->current->type == H_TOKEN_SEMICOLON) {
        ++parser->current;
        return node;
    }
    node->type = AST_NODE_STATEMENT_RETURN_VALUE;
    node->expression.left = parse_expression(parser, OP_PREC_LOWEST);
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected semicolon after ret statement");
    return node;
}

static ast_node_t* parse_repeat_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_REPEAT);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = parse_expression(parser, OP_PREC_LOWEST);
    node->expression.other = parser->temp_one;
    assert_token_type_no_advance(parser, H_TOKEN_LEFT_CURLY, "Expected {.");
    node->expression.right = parse_block_statement(parser);
    return node;
}

static ast_node_t* parse_stop_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_STOP);
    node->operator = parser->current;
    if((++parser->current)->type == H_TOKEN_SEMICOLON) {
        ++parser->current;
        return node;
    }
    node->type = AST_NODE_STATEMENT_STOP_VALUE;
    node->expression.left = parse_expression(parser, OP_PREC_HIGHEST);
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected semicolon after stop statement");
    return node;
}

static ast_node_t* parse_loop_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_LOOP);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = parse_expression(parser, OP_PREC_HIGHEST);
    ast_node_t* temp = ast_node_create(AST_NODE_TEMP_CONTAINER);
    temp->expression.left = parser->temp_one;
    temp->expression.right = parser->temp_two;
    node->expression.other = temp;
    assert_token_type_no_advance(parser, H_TOKEN_LEFT_CURLY, "Expected {.");
    node->expression.right = parse_block_statement(parser);
    return node;
}

static ast_node_t* parse_block_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create_statement_block(AST_NODE_STATEMENT_BLOCK);
    node->operator = parser->current;
    ++parser->current;
    node->block.declarations_capacity = H_BLOCK_DECLARATIONS_CAPACITY;
    node->block.declarations = (ast_node_t**)malloc(sizeof(ast_node_t*) * node->block.declarations_capacity);
    while(parser->current->type != H_TOKEN_RIGHT_CURLY) {
        if(node->block.declarations_size >= node->block.declarations_capacity) {
            node->block.declarations_capacity *= 2;
            node->block.declarations = realloc(node->block.declarations, sizeof(ast_node_t*) * node->block.declarations_capacity);
        }
       
        node->block.declarations[node->block.declarations_size++] = parse_declaration(parser);
    }
    assert_token_type(parser, H_TOKEN_RIGHT_CURLY, "Expected }.");
    return node;
}

static ast_node_t* parse_data_initialisation(parser_t* parser, operator_precedence_t precedence) {
    ast_node_t* node = ast_node_create_statement_block(AST_NODE_DATA_INITIALISATION);
    node->operator = parser->current;
    ++parser->current;
    node->block.declarations_capacity = H_BLOCK_DECLARATIONS_CAPACITY;
    node->block.declarations = (ast_node_t**)malloc(sizeof(ast_node_t*) * node->block.declarations_capacity);
    do {
        if(node->block.declarations_size >= node->block.declarations_capacity) {
            node->block.declarations_capacity *= 2;
            node->block.declarations = realloc(node->block.declarations, sizeof(ast_node_t*) * node->block.declarations_capacity);
        }
        node->block.declarations[node->block.declarations_size++] = parse_expression(parser, OP_PREC_LOWEST);
    } while(parser->current->type == H_TOKEN_COMMA && ++parser->current);
    node->value = VALUE_TYPE(NULL);
    assert_token_type(parser, H_TOKEN_RIGHT_CURLY, "Expected }.");
    return node;
}

static inline ast_node_t* parse_expression_statement(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_EXPRESSION); 
    node->expression.left = parse_expression(parser, OP_PREC_LOWEST);
    assert_token_type(parser, H_TOKEN_SEMICOLON, "Expected semicolon.");
    return node;
}

static inline ast_node_t* parse_expression_statement_do_while(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_NODE_STATEMENT_EXPRESSION); 
    node->expression.left = parse_expression(parser, OP_PREC_LOWEST);
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

static ast_node_t* parse_char(parser_t* parser, operator_precedence_t precedence) {
    ast_node_t* node = ast_node_create(AST_NODE_LITERAL);
    node->operator = parser->current;
    node->value = CHAR_VALUE(*node->operator->start);
    ++parser->current;
    return node;
} 

static ast_node_t* parse_identifier(parser_t* parser, operator_precedence_t precedence) {
    ast_node_t* node = ast_node_create(AST_NODE_IDENTIFIER);
    node->operator = parser->current;
    h_string_t* value = h_string_init_hash(node->operator->start, node->operator->length);
    node->value = STR_VALUE(value);
    ++parser->current;
    return node;
}

/* static ast_node_t* parse_identifier_type(parser_t* parser, operator_precedence_t precedence, ast_node_t* left) {
    ast_node_t* node = ast_node_create(AST_NODE_IDENTIFIER);
    left->type = AST_NODE_IDENTIFIER_TYPE;
    left->expression.left = node;
    left->value.type = H_VALUE_TYPE;
    node->operator = parser->current;
    h_string_t* value = h_string_init_hash(node->operator->start, node->operator->length);
    node->value = STR_VALUE(value);
    ++parser->current;
    assert_token_type(parser, H_TOKEN_EQUAL, "Expected = before data initialisation");
    left->expression.right = parse_expression(parser, OP_PREC_LOWEST);
    return left;
} */

static ast_node_t* parse_identifier_function(parser_t* parser, operator_precedence_t precedence) {
    ast_node_t* node = ast_node_create(AST_NODE_IDENTIFIER_FUNCTION);
    node->operator = parser->current;
    h_string_t* value = h_string_init_hash(node->operator->start, node->operator->length);
    node->value = STR_VALUE(value);
    ++parser->current;
    return node;
}

static ast_node_t* parse_identifier_global(parser_t* parser, operator_precedence_t precedence) {
    ast_node_t* node = ast_node_create(AST_NODE_IDENTIFIER_GLOBAL);
    ++parser->current;
    node->operator = parser->current;
    h_string_t* value = h_string_init_hash(node->operator->start, node->operator->length);
    node->value = STR_VALUE(value);
    ++parser->current;
    return node;
}

static ast_node_t* parse_unary_expression(parser_t* parser, operator_precedence_t precedence) {
    ast_node_t* node = ast_node_create(AST_NODE_UNARY);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = parse_expression(parser, precedence);
    return node;
}

static ast_node_t* parse_unary_identifier_expression(parser_t* parser, operator_precedence_t precedence) {
    ast_node_t* node = ast_node_create(AST_NODE_UNARY);
    node->operator = parser->current;
    ++parser->current;
    assert_token_type_no_advance(parser, H_TOKEN_IDENTIFIER, "Expected identifier after unary operator");
    node->expression.left = parse_identifier(parser, precedence);
    return node;
}

static ast_node_t* parse_post_unary_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left) {
    ast_node_t* node = ast_node_create(AST_NODE_POST_UNARY);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = left;
    return node;
}

static ast_node_t* parse_function_arguments_list_arrow(parser_t* parser, ast_node_t* first_arg) {
    ast_node_t* node = ast_node_create_statement_block(AST_NODE_FUNCTION_ARGUMENTS);
    node->operator = parser->current;
    node->block.declarations_capacity = H_PARAMETERS_LIST_CAPACITY;
    node->block.declarations = (ast_node_t**)malloc(sizeof(ast_node_t*) * node->block.declarations_capacity);
    node->block.declarations[node->block.declarations_size++] = first_arg;
    if(parser->current->type == H_TOKEN_RIGHT_PAR) return node;
    do {
        if(node->block.declarations_size >= node->block.declarations_capacity) {
            node->block.declarations_capacity *= 2;
            node->block.declarations = (ast_node_t**)realloc(node->block.declarations, sizeof(ast_node_t*) * node->block.declarations_capacity);
        }
        node->block.declarations[node->block.declarations_size++] = parse_expression(parser, OP_PREC_LOWEST);
    } while(parser->current->type == H_TOKEN_COMMA && ++parser->current);
    return node;
}

static ast_node_t* parse_arrow_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left) {
    ++parser->current;
    ast_node_t* identifier_node = parse_identifier(parser, OP_PREC_HIGHEST);
    ast_node_t* call_node = parse_function_call_arrow(parser, identifier_node, left);
    return call_node;
}

static ast_node_t* parse_indexing(parser_t* parser, operator_precedence_t precedence, ast_node_t* left) {
    ast_node_t* node = ast_node_create(AST_NODE_INDEXING);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = left;
    node->expression.right = parse_expression(parser, OP_PREC_LOWEST);
    assert_token_type(parser, H_TOKEN_RIGHT_SQUARE, "Expected ]");
    return node;
}

static ast_node_t* parse_function_arguments_list(parser_t* parser) {
    ast_node_t* node = ast_node_create_statement_block(AST_NODE_FUNCTION_ARGUMENTS);
    node->operator = parser->current;
    if(parser->current->type == H_TOKEN_RIGHT_PAR) return node;
    node->block.declarations_capacity = H_PARAMETERS_LIST_CAPACITY;
    node->block.declarations = (ast_node_t**)malloc(sizeof(ast_node_t*) * node->block.declarations_capacity);
    do {
        if(node->block.declarations_size >= node->block.declarations_capacity) {
            node->block.declarations_capacity *= 2;
            node->block.declarations = (ast_node_t**)realloc(node->block.declarations, sizeof(ast_node_t*) * node->block.declarations_capacity);
        }
        node->block.declarations[node->block.declarations_size++] = parse_expression(parser, OP_PREC_LOWEST);
    } while(parser->current->type == H_TOKEN_COMMA && ++parser->current);
    return node;
}

static ast_node_t* parse_function_parameters_list(parser_t* parser) {
    ast_node_t* node = ast_node_create_statement_block(AST_NODE_FUNCTION_PARAMETERS);
    node->operator = parser->current;
    if(parser->current->type == H_TOKEN_RIGHT_PAR) return node;
    node->block.declarations_capacity = H_PARAMETERS_LIST_CAPACITY;
    node->block.declarations = (ast_node_t**)malloc(sizeof(ast_node_t*) * node->block.declarations_capacity);
    do {
        if(node->block.declarations_size >= node->block.declarations_capacity) {
            node->block.declarations_capacity *= 2;
            node->block.declarations = (ast_node_t**)realloc(node->block.declarations, sizeof(ast_node_t*) * node->block.declarations_capacity);
        }
        node->block.declarations[node->block.declarations_size++] = parse_expression(parser, OP_PREC_LOWEST);
    } while(parser->current->type == H_TOKEN_COMMA && ++parser->current);
    return node;
}

static ast_node_t* parse_function_call_arrow(parser_t* parser, ast_node_t* left, ast_node_t* first_arg) {
    ast_node_t* node = ast_node_create(AST_NODE_FUNCTION_CALL);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = left;
    node->expression.right = parse_function_arguments_list_arrow(parser, first_arg);
    assert_token_type(parser, H_TOKEN_RIGHT_PAR, "Expected )");
    return node;
}

static ast_node_t* parse_function_call(parser_t* parser, operator_precedence_t precedence, ast_node_t* left) {
    ast_node_t* node = ast_node_create(AST_NODE_FUNCTION_CALL);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = left;
    node->expression.right = parse_function_arguments_list(parser);
    assert_token_type(parser, H_TOKEN_RIGHT_PAR, "Expected )");
    return node;
}

static ast_node_t* parse_grouping(parser_t* parser, operator_precedence_t precedence) {
    ++parser->current;
    ast_node_t* node = ast_node_create(AST_NODE_GROUPING);
    node->expression.left = parse_expression(parser, OP_PREC_LOWEST);
    assert_token_type(parser, H_TOKEN_RIGHT_PAR, "Expected )");
    return node;
} 

static ast_node_t* parse_array_initialisation(parser_t* parser, operator_precedence_t precedence) {
    ast_node_t* node = ast_node_create_statement_block(AST_NODE_ARRAY_INITIALISATION);
    node->operator = parser->current;
    ++parser->current;
    if(parser->current->type == H_TOKEN_RIGHT_SQUARE) {
        ++parser->current;
        return node;
    }
    node->block.declarations_capacity = H_BLOCK_DECLARATIONS_CAPACITY;
    node->block.declarations = (ast_node_t**)malloc(sizeof(ast_node_t*) * node->block.declarations_capacity);

    current_parse_table = parse_table_array_initialisation;

    do {
        if(node->block.declarations_size >= node->block.declarations_capacity) {
            node->block.declarations_capacity *= 2;
            node->block.declarations = realloc(node->block.declarations, sizeof(ast_node_t*) * node->block.declarations_capacity);
        }
       
        node->block.declarations[node->block.declarations_size++] = parse_expression(parser, OP_PREC_LOWEST);
    } while(parser->current->type == H_TOKEN_COMMA && ++parser->current);
    
    current_parse_table = parse_table;
    node->value = VALUE_ARRAY(NULL);
    assert_token_type(parser, H_TOKEN_RIGHT_SQUARE, "Expected ]");
    return node;
} 

static ast_node_t* parse_expression(parser_t* parser, operator_precedence_t precedence) {
    
    //parse_rule_t* nud = &parse_table[parser->current->type];
    parse_rule_t* nud = &current_parse_table[parser->current->type];

    if(nud->null_denotation == NULL) {
        emit_error(parser, "Expected expression.");
        return ast_node_create(AST_NODE_ERROR);
    }

    ast_node_t* left = nud->null_denotation(parser, nud->precedence); 

    parse_rule_t* led = &current_parse_table[parser->current->type];

    while(led->precedence > precedence) {
        if(led->left_denotation == NULL) { 
            emit_error(parser, "Expected left denotation\n");
        }
        left = led->left_denotation(parser, led->precedence, left);
        led = &current_parse_table[parser->current->type];
    }
    
    return left;
}

static ast_node_t* parse_binary_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left) {
    ast_node_t* node = ast_node_create(AST_NODE_BINARY);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = left;
    node->expression.right = parse_expression(parser, precedence);
    return node;
}

static ast_node_t* parse_ternary_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left) {
    ast_node_t* node = ast_node_create(AST_NODE_TERNARY);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = left;
    node->expression.right = parse_expression(parser, precedence);
    assert_token_type(parser, H_TOKEN_COLON, "Expected : in ternary conditional expression");
    node->expression.other = parse_expression(parser, precedence);
    return node;
}

static ast_node_t* parse_dot_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left) {
    ast_node_t* node = ast_node_create(AST_NODE_DOT);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = left;
    node->expression.right = parse_expression(parser, precedence);
    return node;
}

static ast_node_t* parse_to_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left) {
    ast_node_t* node = ast_node_create(AST_NODE_TO);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = left;
    node->expression.right = parse_expression(parser, precedence);
    return node;
}

static ast_node_t* parse_enum_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left) {
    ast_node_t* node = ast_node_create(AST_NODE_ENUM_RESOLUTION);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = left;
    node->expression.right = parse_identifier(parser, precedence);
    return node;
}

static ast_node_t* parse_assignment_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left) {
    ast_node_t* node = ast_node_create(AST_NODE_ASSIGNMENT);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = left;
    node->expression.right = parse_expression(parser, precedence);
    return node;
}

static ast_node_t* parse_compound_assignment_expression(parser_t* parser, operator_precedence_t precedence, ast_node_t* left) {
    ast_node_t* node = ast_node_create(AST_NODE_ASSIGNMENT_COMPOUND);
    node->operator = parser->current;
    ++parser->current;
    node->expression.left = left;
    node->expression.right = parse_expression(parser, precedence);
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

    switch(node->type) {
        case AST_NODE_STATEMENT_BLOCK:
        case AST_NODE_STATEMENT_BLOCK_ENUM:
        case AST_NODE_ARRAY_INITIALISATION:
        case AST_NODE_DATA_INITIALISATION:
        case AST_NODE_FUNCTION_ARGUMENTS:
        case AST_NODE_FUNCTION_PARAMETERS:
            if(node->block.declarations) {
                for(size_t i = 0; i < node->block.declarations_size; ++i) ast_node_free(node->block.declarations[i]);
                free(node->block.declarations);
            }
            break;
        default:
            if(node->expression.left) {
                ast_node_free(node->expression.left);
            }

            if(node->expression.right) {
                ast_node_free(node->expression.right);
            }
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
        case AST_NODE_IDENTIFIER:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_IDENTIFIER %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_IDENTIFIER_FUNCTION:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_IDENTIFIER_FUNCTION %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_IDENTIFIER_NATIVE:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_IDENTIFIER_NATIVE %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_IDENTIFIER_GLOBAL:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_IDENTIFIER_GLOBAL %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_BINARY:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_BINARY %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_TERNARY:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_TERNARY %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_TO:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_TO %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_ENUM_RESOLUTION:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_ENUM_RESOLUTION %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_ARRAY_INITIALISATION:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_ARRAY_INITIALISATION %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_ASSIGNMENT:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_ASSIGNMENT %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_ASSIGNMENT_COMPOUND:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_ASSIGNMENT_COMPOUND %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_UNARY:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_UNARY %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_POST_UNARY:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_POST_UNARY %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_INDEXING:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_INDEXING %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_FUNCTION_ARGUMENTS:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_FUNCTION_ARGUMENTS %.*s\n", indent, 0, "");
            break;
        case AST_NODE_FUNCTION_PARAMETERS:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_FUNCTION_PARAMETERS %.*s\n", indent, 0, "");
            break;
        case AST_NODE_FUNCTION_CALL:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_FUNCTION_CALL %.*s\n", indent, 0, "");
            break;
        case AST_NODE_NATIVE_CALL:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_NATIVE_CALL %.*s\n", indent, 0, "");
            break;
        case AST_NODE_STATEMENT_PRINT:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_PRINT %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_STATEMENT_IF:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_IF %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_STATEMENT_WHILE:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_WHILE %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_STATEMENT_DO_WHILE:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_DO_WHILE %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_STATEMENT_ASSERTION:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_ASSERTION %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_STATEMENT_BREAK:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_BREAK %.*s\n", indent, 0, "");
            break;
        case AST_NODE_STATEMENT_SKIP:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_SKIP %.*s\n", indent, 0, "");
            break;
        case AST_NODE_STATEMENT_RETURN:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_RETURN %.*s\n", indent, 0, "");
            break;
        case AST_NODE_STATEMENT_RETURN_VALUE:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_RETURN_VALUE %.*s\n", indent, 0, "");
            break;
        case AST_NODE_STATEMENT_STOP:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_STOP %.*s\n", indent, 0, "");
            break;
        case AST_NODE_STATEMENT_STOP_VALUE:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_STOP_VALUE %.*s\n", indent, 0, "");
            break;
        case AST_NODE_STATEMENT_FOR:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_FOR %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_STATEMENT_FOR_CONDITION:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_FOR_CONDITION %.*s\n", indent, (int)node->operator->length, node->operator->start);
            break;
        case AST_NODE_STATEMENT_REPEAT:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_REPEAT %.*s\n", indent, 0, "");
            break;
        case AST_NODE_STATEMENT_LOOP:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_LOOP %.*s\n", indent, 0, "");
            break;
        case AST_NODE_IDENTIFIER_TEMP:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_IDENTIFIER_TEMP %s\n", indent, node->value.string->string);
            break;
        case AST_NODE_IDENTIFIER_TYPE:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_IDENTIFIER_TYPE %s\n", indent, node->value.string->string);
            break;
        case AST_NODE_STATEMENT_BLOCK:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_BLOCK %.*s\n", indent, 0, "");
            break;
        case AST_NODE_STATEMENT_EXPRESSION:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_EXPRESSION %.*s\n", indent, 0, "");
            break;
        case AST_NODE_DECLARATION_VARIABLE:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_DECLARATION_VARIABLE %.*s\n", indent, 0, "");
            break;
        case AST_NODE_DECLARATION_VARIABLE_GLOBAL:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_DECLARATION_VARIABLE_GLOBAL %.*s\n", indent, 0, "");
            break;
        case AST_NODE_DECLARATION_VARIABLE_ARRAY:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_DECLARATION_VARIABLE_ARRAY %.*s\n", indent, 0, "");
            break;
        case AST_NODE_DECLARATION_VARIABLE_TYPE:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_DECLARATION_VARIABLE_TYPE %.*s\n", indent, 0, "");
            break;
        case AST_NODE_DECLARATION_FUNCTION:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_DECLARATION_FUNCTION %.*s\n", indent, 0, "");
            break;
        case AST_NODE_DECLARATION_ENUM:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_DECLARATION_ENUM %.*s\n", indent, 0, "");
            break;
        case AST_NODE_STATEMENT_BLOCK_ENUM:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_BLOCK_ENUM %.*s\n", indent, 0, "");
            break;
        case AST_NODE_DECLARATION_LABEL:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_DECLARATION_LABEL %.*s\n", indent, 0, "");
            break;
        case AST_NODE_STATEMENT_GOTO:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_STATEMENT_GOTO %.*s\n", indent, 0, "");
            break;
        case AST_NODE_TEMP_CONTAINER:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_TEMP_CONTAINER %.*s\n", indent, 0, "");
            break;
        case AST_NODE_DECLARATION_DATA:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_DECLARATION_DATA %.*s\n", indent, 0, "");
            break;
        case AST_NODE_DATA_INITIALISATION:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_DATA_INITIALISATION %.*s\n", indent, 0, "");
            break;
        case AST_NODE_DOT:
            DEBUG_NODE_COLOR(DEBUG_GET_NODE_COLOR(), "%d AST_NODE_DOT %.*s\n", indent, 0, "");
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
    parser->ast_nodes_list = (ast_node_t**)malloc(sizeof(ast_node_t*) * tokens_list_size);
    parser->ast_list_capacity = tokens_list_size;
    parser->ast_list_size = 0;
    parser->tokens_list = tokens_array;
    parser->tokens_list_count = tokens_list_size;
    parser->errors_count = 0;
    parser->temp_one = ast_node_create_temp("_", 1);
    parser->temp_two = ast_node_create_temp("__", 2);
    return parser;
}

void parser_free(parser_t* parser) {
    for(size_t i = 0; i < parser->ast_list_size; ++i) {
        ast_node_free(parser->ast_nodes_list[i]);
    }
    ast_node_free(parser->temp_one);
    ast_node_free(parser->temp_two);
    free(parser->ast_nodes_list);
    free(parser);
}

void ast_print(ast_node_t* node, int indent) {
    disassemble_ast_node(node, indent);
    switch(node->type) {
        case AST_NODE_STATEMENT_BLOCK:
        case AST_NODE_STATEMENT_BLOCK_ENUM:
        case AST_NODE_ARRAY_INITIALISATION:
        case AST_NODE_DATA_INITIALISATION:
        case AST_NODE_FUNCTION_ARGUMENTS:
        case AST_NODE_FUNCTION_PARAMETERS:
            if(node->block.declarations) {
                for(size_t i = 0; i < node->block.declarations_size; ++i) ast_print(node->block.declarations[i], indent + 1);
            }
            break;
        case AST_NODE_DECLARATION_VARIABLE_TYPE:
            ast_print(node->expression.left, indent + 1);
            if(node->expression.right) {
                ast_print(node->expression.right, indent + 1);
            }
            break;
        default:
            if(node->expression.left) {
                ast_print(node->expression.left, indent + 1);
            }
            if(node->expression.right) {
                ast_print(node->expression.right, indent + 1);
            }
            if(node->expression.other) {
                ast_print(node->expression.other, indent + 1);
            }
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

#undef H_BLOCK_DECLARATIONS_CAPACITY
#undef H_PARAMETERS_LIST_CAPACITY
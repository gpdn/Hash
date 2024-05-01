#include "semantic_analyser.h"

#define DEBUG_LOG_UNIMPLEMENTED(node_type, node) \
            DEBUG_COLOR_SET(COLOR_RED); \
            DEBUG_LOG("Add case to semantic analyser - %s - %s\n", node_type, resolve_token_type(node->operator->type));\
            DEBUG_COLOR_RESET();

static void emit_error(semantic_analyser_t* analyser);
static inline void assert_value_type(semantic_analyser_t* analyser, value_type_t value_type, value_type_t type);
static value_type_t resolve_expression(semantic_analyser_t* analyser, ast_node_t* node);
static value_type_t resolve_expression_binary(semantic_analyser_t* analyser, ast_node_t* node);
static value_type_t resolve_expression_unary(semantic_analyser_t* analyser, ast_node_t* node);
static void resolve_ast(semantic_analyser_t* analyser, ast_node_t* node);

static void emit_error(semantic_analyser_t* analyser) {
    DEBUG_LOG("Error\n");
    ++analyser->errors_count;
    analyser->current = analyser->ast_nodes_list[analyser->ast_nodes_list_count - 1];
}

static inline void assert_value_type(semantic_analyser_t* analyser, value_type_t value_type, value_type_t type) {
    if(value_type != type) emit_error(analyser);
}

semantic_analyser_t* h_sa_init(ast_node_t** ast_nodes_list, size_t ast_nodes_list_count, h_hash_table_t* symbol_table) {
    semantic_analyser_t* analyser = (semantic_analyser_t*)malloc(sizeof(semantic_analyser_t));
    analyser->ast_nodes_list = ast_nodes_list;
    analyser->ast_nodes_list_count = ast_nodes_list_count;
    analyser->symbol_table = symbol_table;
    analyser->errors_count = 0;
    return analyser;
}

void h_sa_run(semantic_analyser_t* analyser) {
    analyser->current = *analyser->ast_nodes_list++;
    while(analyser->current->type != AST_NODE_EOF) {
        //ast_print(analyser->current, 0);
        //disassemble_ast_node(analyser->current, 0);
        resolve_ast(analyser, analyser->current);
        analyser->current = *analyser->ast_nodes_list++;
    }
}

static void resolve_ast(semantic_analyser_t* analyser, ast_node_t* node) {
    switch(node->type) {
        case AST_NODE_DECLARATION_VARIABLE:
            if(node->value.type != resolve_expression(analyser, node->right)) emit_error(analyser);
            return;
        case AST_NODE_STATEMENT_PRINT:
            resolve_expression(analyser, node->left);
            return;
        case AST_NODE_STATEMENT_EXPRESSION:
            resolve_expression(analyser, node->left);
            return;
        default:
            resolve_expression(analyser, node);
    }
}

static value_type_t resolve_expression_binary(semantic_analyser_t* analyser, ast_node_t* node) {
    value_type_t value_left = resolve_expression(analyser, node->left);
    value_type_t value_right = resolve_expression(analyser, node->right);

    //if(value_left != value_right) emit_error(analyser);
    
    node->value.type = value_left;
    return node->value.type;
}

static value_type_t resolve_expression_unary(semantic_analyser_t* analyser, ast_node_t* node) {
    value_type_t value = resolve_expression(analyser, node->left);

    switch(node->operator->type) {
        case H_TOKEN_BITWISE_NOT:
        case H_TOKEN_MINUS:
            assert_value_type(analyser, value, H_VALUE_NUMBER);
            break;
        default:
            DEBUG_LOG_UNIMPLEMENTED("Unary Expression", node);
            break;
    }
    
    return value;
}

static value_type_t resolve_expression(semantic_analyser_t* analyser, ast_node_t* node) {
    switch(node->type) {
        case AST_NODE_BINARY:
        case AST_NODE_ASSIGNMENT:
            return resolve_expression_binary(analyser, node);
        case AST_NODE_LITERAL:
        case AST_NODE_IDENTIFIER:
            return node->value.type;
        case AST_NODE_UNARY:
            return resolve_expression_unary(analyser, node);
        default:
            emit_error(analyser);
            return H_VALUE_NULL;
            break;
    }
}

void h_sa_free(semantic_analyser_t* analyser) {
    free(analyser);
}

#undef DEBUG_LOG_UNIMPLEMENTED
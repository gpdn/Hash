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

semantic_analyser_t* h_sa_init(ast_node_t** ast_nodes_list, size_t ast_nodes_list_count, h_hash_table_t* globals_table, h_locals_stack_t* locals_stack, h_ht_labels_t* labels_table) {
    semantic_analyser_t* analyser = (semantic_analyser_t*)malloc(sizeof(semantic_analyser_t));
    analyser->ast_nodes_list = ast_nodes_list;
    analyser->ast_nodes_list_count = ast_nodes_list_count;
    analyser->globals_table = globals_table;
    analyser->errors_count = 0;
    analyser->scope = 0;
    analyser->locals = locals_stack;
    analyser->labels_table = labels_table;
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
        case AST_NODE_DECLARATION_VARIABLE_CONSTANT:
            if(node->value.type != resolve_expression(analyser, node->expression.right)) emit_error(analyser);
            h_locals_stack_push(analyser->locals, node->expression.left->value.string, node->expression.right->value);
            h_locals_stack_print(analyser->locals);
            return;
        case AST_NODE_DECLARATION_VARIABLE_GLOBAL:
            if(node->value.type != resolve_expression(analyser, node->expression.right)) emit_error(analyser);
            h_ht_set(analyser->globals_table, node->expression.left->value.string, node->expression.right->value);
            //h_ht_print(analyser->globals_table);
            return;
        case AST_NODE_DECLARATION_LABEL:
            return;
        case AST_NODE_STATEMENT_PRINT:
            resolve_expression(analyser, node->expression.left);
            return;
        case AST_NODE_STATEMENT_BLOCK:
            ++analyser->scope;
            for(size_t i = 0; i < node->block.declarations_size; ++i) {
                resolve_ast(analyser, node->block.declarations[i]);
            }
            --analyser->scope;
            return;
        case AST_NODE_STATEMENT_EXPRESSION:
            resolve_expression(analyser, node->expression.left);
            return;
        default:
            resolve_expression(analyser, node);
    }
}

static value_type_t resolve_expression_binary(semantic_analyser_t* analyser, ast_node_t* node) {
    value_type_t value_left = resolve_expression(analyser, node->expression.left);
    value_type_t value_right = resolve_expression(analyser, node->expression.right);

    switch(node->operator->type) {
        case H_TOKEN_BITWISE_AND:
        case H_TOKEN_BITWISE_OR:
        case H_TOKEN_BITWISE_XOR:
        case H_TOKEN_BITWISE_SHIFT_LEFT:
        case H_TOKEN_BITWISE_SHIFT_RIGHT:
        case H_TOKEN_MINUS:
        case H_TOKEN_STAR:
        case H_TOKEN_SLASH:
            assert_value_type(analyser, value_left, H_VALUE_NUMBER);
            assert_value_type(analyser, value_right, H_VALUE_NUMBER);
            break;
        case H_TOKEN_PLUS:
            assert_value_type(analyser, value_left, value_right);
            break;
        case H_TOKEN_DOUBLE_EQUAL:
            assert_value_type(analyser, value_left, value_right);
            node->value.type = H_VALUE_NUMBER;
            return H_VALUE_NUMBER;
            break;
        default:
            //if(value_left != value_right) emit_error(analyser);
            break;
    }
    
    node->value.type = value_left;
    return node->value.type;
}

static value_type_t resolve_expression_unary(semantic_analyser_t* analyser, ast_node_t* node) {
    value_type_t value = resolve_expression(analyser, node->expression.left);

    switch(node->operator->type) {
        case H_TOKEN_BITWISE_NOT:
        case H_TOKEN_MINUS:
        //case H_TOKEN_PLUS_PLUS:
        //case H_TOKEN_MINUS_MINUS:
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
        case AST_NODE_IDENTIFIER_GLOBAL:
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
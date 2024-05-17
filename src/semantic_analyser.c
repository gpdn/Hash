#include "semantic_analyser.h"

#define DEBUG_LOG_UNIMPLEMENTED(node_type, node) \
            DEBUG_COLOR_SET(COLOR_RED); \
            DEBUG_LOG("Add case to semantic analyser - %s - %s\n", node_type, resolve_token_type(node->operator->type));\
            DEBUG_COLOR_RESET();

static void emit_error(semantic_analyser_t* analyser);
static inline void assert_value_type(semantic_analyser_t* analyser, value_type_t value_type, value_type_t type);
static inline void assert_iterable(semantic_analyser_t* analyser, value_type_t value_type);
static value_t resolve_expression(semantic_analyser_t* analyser, ast_node_t* node);
static value_t resolve_expression_binary(semantic_analyser_t* analyser, ast_node_t* node);
static inline value_t resolve_expression_indexing(semantic_analyser_t* analyser, ast_node_t* node);
static value_t resolve_expression_unary(semantic_analyser_t* analyser, ast_node_t* node);
static value_t resolve_expression_post_unary(semantic_analyser_t* analyser, ast_node_t* node);
static inline void resolve_expression_array_initialisation(semantic_analyser_t* analyser, ast_node_t* node, value_type_t type);
static void resolve_ast(semantic_analyser_t* analyser, ast_node_t* node);
static inline void resolve_block_statement(semantic_analyser_t* analyser, ast_node_t* node);
static void ast_post_check_push(semantic_analyser_t* analyser, ast_node_t* node);

static value_type_t iterables[] = {
    [H_VALUE_UNDEFINED] = 0,
    [H_VALUE_NULL] = 0,
    [H_VALUE_CHAR] = 0,
    [H_VALUE_NUMBER] = 0,
    [H_VALUE_STRING] = 1,
    [H_VALUE_ARRAY] = 1
};

static void ast_post_check_push(semantic_analyser_t* analyser, ast_node_t* node) {
    if(analyser->ast_nodes_post_checks_size >= analyser->ast_nodes_post_checks_capacity) {
        analyser->ast_nodes_post_checks_capacity *= 2;
        analyser->ast_nodes_post_checks = (ast_node_t**)realloc(analyser->ast_nodes_post_checks, sizeof(ast_node_t*) * analyser->ast_nodes_post_checks_capacity);
    }

    analyser->ast_nodes_post_checks[analyser->ast_nodes_post_checks_size++] = node;
}

static void emit_error(semantic_analyser_t* analyser) {
    DEBUG_LOG("Error\n");
    ++analyser->errors_count;
    analyser->current = analyser->ast_nodes_list[analyser->ast_nodes_list_count - 1];
}

static inline void assert_iterable(semantic_analyser_t* analyser, value_type_t value_type) {
    if(iterables[value_type] == 0) emit_error(analyser);
}

static inline void assert_value_type(semantic_analyser_t* analyser, value_type_t value_type, value_type_t type) {
    if(value_type != type) emit_error(analyser);
}

static inline void assert_ast_node_type(semantic_analyser_t* analyser, ast_node_t* node, ast_node_type_t type) {
    if(node->type != type) emit_error(analyser);
}

semantic_analyser_t* h_sa_init(ast_node_t** ast_nodes_list, size_t ast_nodes_list_count, h_hash_table_t* globals_table, h_locals_stack_t* locals_stack, h_ht_labels_t* labels_table, h_ht_enums_t* enums_table) {
    semantic_analyser_t* analyser = (semantic_analyser_t*)malloc(sizeof(semantic_analyser_t));
    analyser->ast_nodes_list = ast_nodes_list;
    analyser->ast_nodes_list_count = ast_nodes_list_count;
    analyser->globals_table = globals_table;
    analyser->errors_count = 0;
    analyser->scope = 0;
    analyser->loop_count = 0;
    analyser->locals = locals_stack;
    analyser->labels_table = labels_table;
    analyser->enums_table = enums_table;
    analyser->ast_nodes_post_checks_size = 0;
    analyser->ast_nodes_post_checks_capacity = 5;
    analyser->ast_nodes_post_checks = (ast_node_t**)malloc(sizeof(ast_node_t*) * analyser->ast_nodes_post_checks_capacity);
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

    for(size_t i = 0; i < analyser->ast_nodes_post_checks_size; ++i) {
        DEBUG_LOG("Post Check %lld - %s\n", i, analyser->ast_nodes_post_checks[i]->expression.left->value.string->string);
        h_ht_labels_get(analyser->labels_table, analyser->ast_nodes_post_checks[i]->expression.left->value.string);
    }
}

static void resolve_ast(semantic_analyser_t* analyser, ast_node_t* node) {
    switch(node->type) {
        case AST_NODE_DECLARATION_VARIABLE:
        case AST_NODE_DECLARATION_VARIABLE_CONSTANT:
            value_t rvalue_local = resolve_expression(analyser, node->expression.right);
            if(node->value.type != rvalue_local.type) emit_error(analyser);
            h_locals_stack_push(analyser->locals, node->expression.left->value.string, rvalue_local, analyser->scope);
            //h_locals_stack_print(analyser->locals);
            return;
        case AST_NODE_DECLARATION_VARIABLE_ARRAY:
            if(node->expression.right->type == AST_NODE_ARRAY_INITIALISATION) {
                resolve_expression_array_initialisation(analyser, node->expression.right, node->value.type);
                h_locals_stack_push(analyser->locals, node->expression.left->value.string, node->expression.right->value, analyser->scope);
                return;
            }
            resolve_expression(analyser, node->expression.right);
            h_locals_stack_push(analyser->locals, node->expression.left->value.string, node->expression.right->value, analyser->scope);
            h_locals_stack_print(analyser->locals);
            return;
        case AST_NODE_DECLARATION_VARIABLE_GLOBAL:
            value_t rvalue_global = resolve_expression(analyser, node->expression.right); 
            if(node->value.type != rvalue_global.type) emit_error(analyser);
            h_ht_set(analyser->globals_table, node->expression.left->value.string, rvalue_global);
            //h_ht_print(analyser->globals_table);
            return;
        case AST_NODE_DECLARATION_LABEL:
            h_ht_labels_set(analyser->labels_table, node->expression.left->value.string, 0);
            return;
        case AST_NODE_DECLARATION_ENUM:
            ht_enum_t* values = h_ht_enums_set(analyser->enums_table, node->expression.left->value.string);
            for(size_t i = 0; i < node->expression.right->block.declarations_size; ++i) {
                h_ht_enum_value_set(values, node->expression.right->block.declarations[i]->value.string);
            } 
            h_ht_enums_print(analyser->enums_table);
            return;
        case AST_NODE_STATEMENT_PRINT:
            resolve_expression(analyser, node->expression.left);
            return;
        case AST_NODE_STATEMENT_IF:
            if(resolve_expression(analyser, node->expression.left).type == H_VALUE_UNDEFINED) emit_error(analyser);
            resolve_block_statement(analyser, node->expression.right);
            if(node->expression.other) resolve_block_statement(analyser, node->expression.other);
            return;
        case AST_NODE_STATEMENT_GOTO:
            //if(resolve_expression(analyser, node->expression.left).type == H_VALUE_UNDEFINED) emit_error(analyser);
            ast_post_check_push(analyser, node);
            return;
        case AST_NODE_STATEMENT_WHILE:
            if(resolve_expression(analyser, node->expression.left).type == H_VALUE_UNDEFINED) emit_error(analyser);
            resolve_block_statement(analyser, node->expression.right);
            return;
        case AST_NODE_STATEMENT_DO_WHILE:
            if(resolve_expression(analyser, node->expression.right).type == H_VALUE_UNDEFINED) emit_error(analyser);
            resolve_block_statement(analyser, node->expression.left);
            return;
        case AST_NODE_STATEMENT_ASSERTION:
            resolve_expression(analyser, node->expression.left);
            resolve_ast(analyser, node->expression.right);
            return;
        case AST_NODE_STATEMENT_FOR_CONDITION:
            if(node->expression.left) resolve_ast(analyser, node->expression.left);
            resolve_expression(analyser, node->expression.other);
            resolve_expression(analyser, node->expression.right);
            return;
        case AST_NODE_STATEMENT_FOR:
            ++analyser->scope;
            resolve_ast(analyser, node->expression.left);
            --analyser->scope;
            resolve_block_statement(analyser, node->expression.right);
            h_locals_stack_print(analyser->locals);
            return;
        case AST_NODE_STATEMENT_REPEAT:
            resolve_ast(analyser, node->expression.left);
            assert_value_type(analyser, node->expression.left->value.type, H_VALUE_NUMBER);
            h_locals_stack_push(analyser->locals, node->expression.other->value.string, NUM_VALUE(0), analyser->scope + 1);
            resolve_block_statement(analyser, node->expression.right);
            return;
        case AST_NODE_STATEMENT_LOOP:
            if(resolve_expression(analyser, node->expression.left).type == H_VALUE_UNDEFINED) emit_error(analyser);
            assert_iterable(analyser, node->expression.left->value.type);
            h_locals_stack_push(analyser->locals, node->expression.other->value.string, NUM_VALUE(0), analyser->scope + 1);
            resolve_block_statement(analyser, node->expression.right);
            return;
        case AST_NODE_STATEMENT_BLOCK:
            resolve_block_statement(analyser, node);
            return;
        case AST_NODE_STATEMENT_EXPRESSION:
            ast_print(node->expression.left, 0);
            resolve_expression(analyser, node->expression.left);
            return;
        default:
            resolve_expression(analyser, node);
    }
}

static inline value_t resolve_expression_indexing(semantic_analyser_t* analyser, ast_node_t* node) {
    value_t value_left = resolve_expression(analyser, node->expression.left);
    value_t value_right = resolve_expression(analyser, node->expression.right);
    assert_iterable(analyser, value_left.type);
    assert_value_type(analyser, value_right.type, H_VALUE_NUMBER);
    node->value.type = value_left.array->type;
    return node->value;
}

static inline void resolve_expression_array_initialisation(semantic_analyser_t* analyser, ast_node_t* node, value_type_t type) {
    for(size_t i = 0; i < node->block.declarations_size; ++i) {
        value_t value = resolve_expression(analyser, node->block.declarations[i]);
        assert_value_type(analyser, value.type, type);
    }
    node->value.array->type = type;
}

static value_t resolve_expression_binary(semantic_analyser_t* analyser, ast_node_t* node) {
    value_t value_left = resolve_expression(analyser, node->expression.left);
    value_t value_right = resolve_expression(analyser, node->expression.right);

    switch(node->operator->type) {
        case H_TOKEN_BITWISE_AND:
        case H_TOKEN_BITWISE_OR:
        case H_TOKEN_BITWISE_XOR:
        case H_TOKEN_BITWISE_SHIFT_LEFT:
        case H_TOKEN_BITWISE_SHIFT_RIGHT:
        case H_TOKEN_MINUS:
        case H_TOKEN_STAR:
        case H_TOKEN_MINUS_EQUAL:
        case H_TOKEN_STAR_EQUAL:
        case H_TOKEN_SLASH_EQUAL:
            assert_value_type(analyser, value_left.type, H_VALUE_NUMBER);
            assert_value_type(analyser, value_right.type, H_VALUE_NUMBER);
            break;
        case H_TOKEN_PLUS:
        case H_TOKEN_PLUS_EQUAL:
            assert_value_type(analyser, value_left.type, value_right.type);
            break;
        case H_TOKEN_EQUAL:
        case H_TOKEN_DOUBLE_EQUAL:
        case H_TOKEN_BANG_EQUAL:
        case H_TOKEN_GREATER:
        case H_TOKEN_GREATER_EQUAL:
        case H_TOKEN_LESS:
        case H_TOKEN_LESS_EQUAL:
            assert_value_type(analyser, value_left.type, value_right.type);
            node->value.type = H_VALUE_NUMBER;
            return node->value;
            break;
        default:
            //if(value_left != value_right) emit_error(analyser);
            emit_error(analyser);
            break;
    }
    
    node->value.type = value_left.type;
    return node->value;
}

static value_t resolve_expression_unary(semantic_analyser_t* analyser, ast_node_t* node) {
    value_t value = resolve_expression(analyser, node->expression.left);

    switch(node->operator->type) {
        case H_TOKEN_BITWISE_NOT:
        case H_TOKEN_MINUS:
        case H_TOKEN_PLUS_PLUS:
        case H_TOKEN_MINUS_MINUS:
            assert_value_type(analyser, value.type, H_VALUE_NUMBER);
            break;
        default:
            DEBUG_LOG_UNIMPLEMENTED("Unary Expression", node);
            break;
    }
    
    return value;
}

static value_t resolve_expression_post_unary(semantic_analyser_t* analyser, ast_node_t* node) {
    value_t value = resolve_expression(analyser, node->expression.left);

    switch(node->operator->type) {
        case H_TOKEN_PLUS_PLUS:
        case H_TOKEN_MINUS_MINUS:
            assert_value_type(analyser, value.type, H_VALUE_NUMBER);
            break;
        default:
            DEBUG_LOG_UNIMPLEMENTED("Unary Expression", node);
            break;
    }
    
    return value;
}

static value_t resolve_expression(semantic_analyser_t* analyser, ast_node_t* node) {
    switch(node->type) {
        case AST_NODE_BINARY:
        case AST_NODE_ASSIGNMENT:
            return resolve_expression_binary(analyser, node);
        case AST_NODE_LITERAL:
            return node->value;
        case AST_NODE_IDENTIFIER:
            return h_locals_stack_get(analyser->locals, node->value.string, analyser->scope);
        case AST_NODE_IDENTIFIER_GLOBAL:
            return h_ht_get(analyser->globals_table, node->value.string);
        case AST_NODE_UNARY:
            return resolve_expression_unary(analyser, node);
        case AST_NODE_POST_UNARY:
            return resolve_expression_post_unary(analyser, node);
        case AST_NODE_ENUM_RESOLUTION:
            return NUM_VALUE(1);
        case AST_NODE_INDEXING:
            return resolve_expression_indexing(analyser, node);
        default:
            emit_error(analyser);
            return UNDEFINED_VALUE(0);
            break;
    }
}

static inline void resolve_block_statement(semantic_analyser_t* analyser, ast_node_t* node) {
    ++analyser->scope;
    for(size_t i = 0; i < node->block.declarations_size; ++i) {
        resolve_ast(analyser, node->block.declarations[i]);
    }
    --analyser->scope;
}

void h_sa_free(semantic_analyser_t* analyser) {
    free(analyser->ast_nodes_post_checks);
    free(analyser);
}

#undef DEBUG_LOG_UNIMPLEMENTED
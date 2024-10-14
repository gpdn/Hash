#include "semantic_analyser.h"

#define DEBUG_LOG_UNIMPLEMENTED(node_type, node) \
            DEBUG_COLOR_SET(COLOR_RED); \
            DEBUG_LOG("Add case to semantic analyser - %s - %s\n", node_type, resolve_token_type(node->operator->type));\
            DEBUG_COLOR_RESET();

#define H_DEFAULT_POST_CHECKS_LIST_CAPACITY 5;
#define H_DEFAULT_RETURNS_LIST_CAPACITY 5;

static void emit_error(semantic_analyser_t* analyser, const char* error);
static inline void assert_value_type(semantic_analyser_t* analyser, value_type_t value_type, value_type_t type);
static inline void assert_iterable(semantic_analyser_t* analyser, value_type_t value_type);
static inline void assert_parameters_arity(semantic_analyser_t* analyser, h_function_t* function, ast_node_t* parameters_list);
static inline void assert_loop_count(semantic_analyser_t* analyser);
static value_t resolve_expression(semantic_analyser_t* analyser, ast_node_t* node);
static value_t resolve_expression_binary(semantic_analyser_t* analyser, ast_node_t* node);
static inline value_t resolve_expression_indexing(semantic_analyser_t* analyser, ast_node_t* node);
static inline value_t resolve_return_statement(semantic_analyser_t* analyser, ast_node_t* node);
static value_t resolve_expression_unary(semantic_analyser_t* analyser, ast_node_t* node);
static value_t resolve_expression_post_unary(semantic_analyser_t* analyser, ast_node_t* node);
static inline void resolve_expression_array_initialisation(semantic_analyser_t* analyser, ast_node_t* node, value_type_t type);
static void resolve_ast(semantic_analyser_t* analyser, ast_node_t* node);
static inline void resolve_block_statement(semantic_analyser_t* analyser, ast_node_t* node);
static void ast_post_check_push(semantic_analyser_t* analyser, ast_node_t* node);
static void returns_list_push(semantic_analyser_t* analyser, value_type_t type);
static inline void resolve_returns_list(semantic_analyser_t* analyser, value_type_t type);
static inline void assert_returns_type(semantic_analyser_t* analyser, value_type_t type);
static inline void assert_returns_undefined(semantic_analyser_t* analyser);

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

static void returns_list_push(semantic_analyser_t* analyser, value_type_t type) {
    if(analyser->returns_list.size >= analyser->returns_list.capacity) {
        analyser->returns_list.capacity *= 2;
        analyser->returns_list.types = (value_type_t*)realloc(analyser->returns_list.types, sizeof(value_type_t) * analyser->returns_list.capacity);
    }

    analyser->returns_list.types[analyser->returns_list.size++] = type;
}

static inline void assert_returns_type(semantic_analyser_t* analyser, value_type_t type) {
    if(analyser->returns_list.size == 0) emit_error(analyser, "Expected return value");
    for(size_t i = 0; i < analyser->returns_list.size; ++i) {
        if(analyser->returns_list.types[i] != type) emit_error(analyser, "Invalid return type");
    }
    analyser->returns_list.size = 0;
}

static inline void assert_returns_undefined(semantic_analyser_t* analyser) {
    for(size_t i = 0; i < analyser->returns_list.size; ++i) {
        if(analyser->returns_list.types[i] != H_VALUE_UNDEFINED) emit_error(analyser, "No return value expected");
    }
    analyser->returns_list.size = 0;
}

static inline void resolve_returns_list(semantic_analyser_t* analyser, value_type_t type) {
    if(type != H_VALUE_UNDEFINED) {
        assert_returns_type(analyser, type);
        return;
    }
    assert_returns_undefined(analyser);
}

static void emit_error(semantic_analyser_t* analyser, const char* error) {
    DEBUG_LOG("Semantic Analyser Error: %s\n", error);
    ++analyser->errors_count;
    analyser->current = analyser->ast_nodes_list[analyser->ast_nodes_list_count - 1];
}

static inline void assert_iterable(semantic_analyser_t* analyser, value_type_t value_type) {
    if(iterables[value_type] == 0) emit_error(analyser, "Expected iterable type");
}

static inline void assert_value_type(semantic_analyser_t* analyser, value_type_t value_type, value_type_t type) {
    if(value_type != type) emit_error(analyser, "Type mismatch");
}

static inline void assert_ast_node_type(semantic_analyser_t* analyser, ast_node_t* node, ast_node_type_t type) {
    if(node->type != type) emit_error(analyser, "Add error to assert_ast_node_type");
}

static inline void assert_parameters_arity(semantic_analyser_t* analyser, h_function_t* function, ast_node_t* parameters_list) {
    if(function->parameters_list_size != parameters_list->block.declarations_size) emit_error(analyser, "Invalid arguments count");
    //print_value(&parameters_list->block.declarations[0]->value);
    
    for(size_t i = 0; i < function->parameters_list_size; ++i) {
        DEBUG_LOG("Iteration %lld\n", i);
        DEBUG_LOG("Func: %s\n", resolve_value_type(function->parameters_list[i].type));
        print_value(&parameters_list->block.declarations[i]->value);
        value_t arg_value = resolve_expression(analyser, parameters_list->block.declarations[i]);
        //parameters_list->block.declarations[i]->value = resolve_expression(analyser, parameters_list->block.declarations[i]); 
        DEBUG_LOG("Args: %s\n", resolve_value_type(parameters_list->block.declarations[i]->value.type));
        print_value(&parameters_list->block.declarations[i]->value);
        DEBUG_LOG("Type Param: %d\n", function->parameters_list[i].type);
        DEBUG_LOG("Type Args: %d\n", parameters_list->block.declarations[i]->value.type);
        if(function->parameters_list[i].type != arg_value.type) emit_error(analyser, "Invalid argument type");
    }
}

static inline void assert_loop_count(semantic_analyser_t* analyser) {
    if(analyser->loop_count == 0) emit_error(analyser, "Cannot use statement outside fo a loop");
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
    analyser->initial_locals = locals_stack;
    analyser->labels_table = labels_table;
    analyser->enums_table = enums_table;
    analyser->ast_nodes_post_checks_size = 0;
    analyser->ast_nodes_post_checks_capacity = H_DEFAULT_POST_CHECKS_LIST_CAPACITY;
    analyser->ast_nodes_post_checks = (ast_node_t**)malloc(sizeof(ast_node_t*) * analyser->ast_nodes_post_checks_capacity);
    analyser->returns_list.capacity = H_DEFAULT_RETURNS_LIST_CAPACITY;
    analyser->returns_list.size = 0;
    analyser->returns_list.types = (value_type_t*)malloc(sizeof(value_type_t) * analyser->returns_list.capacity);
    return analyser;
}

void h_sa_run(semantic_analyser_t* analyser) {
    analyser->current = *analyser->ast_nodes_list++;
    while(analyser->current->type != AST_NODE_EOF) {
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
            if(node->value.type != rvalue_local.type) emit_error(analyser, "Invalid type assignment");
            if(h_locals_stack_find(analyser->locals, node->expression.left->value.string, analyser->scope) == 1) emit_error(analyser, "Variable already defined");
            h_locals_stack_push(analyser->locals, node->expression.left->value.string, rvalue_local, analyser->scope);
            return;
        case AST_NODE_DECLARATION_VARIABLE_ARRAY:
            if(node->expression.right->type == AST_NODE_ARRAY_INITIALISATION) {
                resolve_expression_array_initialisation(analyser, node->expression.right, node->value.type);
                h_locals_stack_push(analyser->locals, node->expression.left->value.string, node->expression.right->value, analyser->scope);
                return;
            }
            resolve_expression(analyser, node->expression.right);
            h_locals_stack_push(analyser->locals, node->expression.left->value.string, node->expression.right->value, analyser->scope);
            return;
        case AST_NODE_DECLARATION_VARIABLE_GLOBAL:
            value_t rvalue_global = resolve_expression(analyser, node->expression.right); 
            if(node->value.type != rvalue_global.type) emit_error(analyser, "Variable already defined");
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
        case AST_NODE_DECLARATION_FUNCTION:
            h_locals_stack_push(analyser->locals, node->expression.left->value.string, node->value, analyser->scope);
            analyser->locals = node->value.function->locals_stack;
            resolve_block_statement(analyser, node->expression.right);
            analyser->locals = analyser->initial_locals;
            resolve_returns_list(analyser, node->value.function->return_type);
            return;
        case AST_NODE_DECLARATION_DATA:
            h_locals_stack_push(analyser->locals, node->expression.left->value.string, node->value, analyser->scope);
            analyser->locals = node->value.function->locals_stack;
            resolve_block_statement(analyser, node->expression.right);
            analyser->locals = analyser->initial_locals;
            resolve_returns_list(analyser, node->value.function->return_type);
            return;
        case AST_NODE_STATEMENT_PRINT:
            if(resolve_expression(analyser, node->expression.left).type == H_VALUE_UNDEFINED) emit_error(analyser, "Undefined variable");
            return;
        case AST_NODE_STATEMENT_BREAK:
        case AST_NODE_STATEMENT_SKIP:
            assert_loop_count(analyser);
            return;
        case AST_NODE_STATEMENT_RETURN:
        case AST_NODE_STATEMENT_RETURN_VALUE:
            resolve_return_statement(analyser, node->expression.left);
            return;
        case AST_NODE_STATEMENT_IF:
            if(resolve_expression(analyser, node->expression.left).type == H_VALUE_UNDEFINED) emit_error(analyser, "Undefined variable");
            resolve_block_statement(analyser, node->expression.right);
            if(node->expression.other) resolve_block_statement(analyser, node->expression.other);
            return;
        case AST_NODE_STATEMENT_GOTO:
            //if(resolve_expression(analyser, node->expression.left).type == H_VALUE_UNDEFINED) emit_error(analyser);
            ast_post_check_push(analyser, node);
            return;
        case AST_NODE_STATEMENT_WHILE:
            ++analyser->loop_count;
            if(resolve_expression(analyser, node->expression.left).type == H_VALUE_UNDEFINED) emit_error(analyser, "Undefined variable");
            resolve_block_statement(analyser, node->expression.right);
            --analyser->loop_count;
            return;
        case AST_NODE_STATEMENT_DO_WHILE:
            ++analyser->loop_count;
            if(resolve_expression(analyser, node->expression.right).type == H_VALUE_UNDEFINED) emit_error(analyser, "Undefined variable");
            resolve_block_statement(analyser, node->expression.left);
            --analyser->loop_count;
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
            ++analyser->loop_count;
            ++analyser->scope;
            resolve_ast(analyser, node->expression.left);
            --analyser->scope;
            resolve_block_statement(analyser, node->expression.right);
            --analyser->loop_count;
            return;
        case AST_NODE_STATEMENT_REPEAT:
            ++analyser->loop_count;
            resolve_ast(analyser, node->expression.left);
            assert_value_type(analyser, node->expression.left->value.type, H_VALUE_NUMBER);
            h_locals_stack_push(analyser->locals, node->expression.other->value.string, NUM_VALUE(0), analyser->scope + 1);
            resolve_block_statement(analyser, node->expression.right);
            --analyser->loop_count;
            return;
        case AST_NODE_STATEMENT_LOOP:
            //ast_print(node->expression.other, 0);
            ++analyser->loop_count;
            if(resolve_expression(analyser, node->expression.left).type == H_VALUE_UNDEFINED) emit_error(analyser, "Undefined variable");
            assert_iterable(analyser, node->expression.left->value.type);
            //h_locals_stack_push(analyser->locals, node->expression.other->value.string, NUM_VALUE(0), analyser->scope + 1);
            h_locals_stack_push(analyser->locals, node->expression.other->expression.left->value.string, NUM_VALUE(0), analyser->scope + 1);
            h_locals_stack_push(analyser->locals, node->expression.other->expression.right->value.string, NUM_VALUE(0), analyser->scope + 1);
            resolve_block_statement(analyser, node->expression.right);
            --analyser->loop_count;
            return;
        case AST_NODE_STATEMENT_BLOCK:
            resolve_block_statement(analyser, node);
            return;
        case AST_NODE_STATEMENT_EXPRESSION:
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

static inline value_t resolve_return_statement(semantic_analyser_t* analyser, ast_node_t* node) {
    if(node) {
        value_t return_value = resolve_expression(analyser, node); 
        returns_list_push(analyser, return_value.type);
        return return_value;
    }
    value_t return_value = UNDEFINED_VALUE(0); 
    returns_list_push(analyser, return_value.type);
    return return_value;
}

static inline value_t resolve_expression_function_call(semantic_analyser_t* analyser, ast_node_t* node) {
    node->expression.left->type = AST_NODE_IDENTIFIER_FUNCTION;
    value_t value_left = resolve_expression(analyser, node->expression.left);
    assert_value_type(analyser, value_left.type, H_VALUE_FUNCTION);
    resolve_block_statement(analyser, node->expression.right);
    assert_parameters_arity(analyser, value_left.function, node->expression.right);
    node->value.type = value_left.function->return_type;
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
        case H_TOKEN_TO:
            //assert_value_type(analyser, value_left.type, H_VALUE_NUMBER);
            assert_value_type(analyser, value_left.type, value_right.type);
            break;
        default:
            //if(value_left != value_right) emit_error(analyser);
            emit_error(analyser, "Undefined node");
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
        case AST_NODE_ASSIGNMENT_COMPOUND:
        case AST_NODE_TO:
            return resolve_expression_binary(analyser, node);
        case AST_NODE_LITERAL:
            return node->value;
        case AST_NODE_IDENTIFIER:
            return h_locals_stack_get(analyser->locals, node->value.string, analyser->scope);
        case AST_NODE_IDENTIFIER_FUNCTION:
            return h_locals_stack_get(analyser->initial_locals, node->value.string, 0);
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
        case AST_NODE_FUNCTION_CALL:
            return resolve_expression_function_call(analyser, node);
        default:
            emit_error(analyser, "Undefined node");
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
    free(analyser->returns_list.types);
    free(analyser);
}

#undef DEBUG_LOG_UNIMPLEMENTED
#undef H_DEFAULT_POST_CHECKS_LIST_CAPACITY
#undef H_DEFAULT_RETURNS_LIST_CAPACITY
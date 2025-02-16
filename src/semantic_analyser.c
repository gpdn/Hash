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
static inline void assert_return_count(semantic_analyser_t* analyser);
static value_t resolve_expression(semantic_analyser_t* analyser, ast_node_t* node);
static value_t resolve_expression_binary(semantic_analyser_t* analyser, ast_node_t* node);
static inline value_t resolve_expression_indexing(semantic_analyser_t* analyser, ast_node_t* node);
static inline value_t resolve_return_statement(semantic_analyser_t* analyser, ast_node_t* node);
static value_t resolve_expression_unary(semantic_analyser_t* analyser, ast_node_t* node);
static value_t resolve_expression_post_unary(semantic_analyser_t* analyser, ast_node_t* node);
static inline value_t resolve_expression_array_initialisation(semantic_analyser_t* analyser, ast_node_t* node);
static inline value_t resolve_expression_data_initialisation(semantic_analyser_t* analyser, ast_node_t* node);
static inline value_t resolve_expression_dot(semantic_analyser_t* analyser, ast_node_t* node);
static void resolve_ast(semantic_analyser_t* analyser, ast_node_t* node);
static inline void resolve_block_statement(semantic_analyser_t* analyser, ast_node_t* node);
static void ast_post_check_push(semantic_analyser_t* analyser, ast_node_t* node);
static void returns_list_push(semantic_analyser_t* analyser, value_t type);
static inline void resolve_returns_list(semantic_analyser_t* analyser, value_t type);
static inline void assert_returns_type(semantic_analyser_t* analyser, value_t type);
static inline void assert_returns_undefined(semantic_analyser_t* analyser);
static inline void assert_type_undefined(semantic_analyser_t* analyser, h_string_t* type);
static inline void assert_type_defined(semantic_analyser_t* analyser, h_string_t* type);

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

static void returns_list_push(semantic_analyser_t* analyser, value_t type) {
    if(analyser->returns_list.size >= analyser->returns_list.capacity) {
        analyser->returns_list.capacity *= 2;
        analyser->returns_list.types = (value_t*)realloc(analyser->returns_list.types, sizeof(value_t) * analyser->returns_list.capacity);
    }

    analyser->returns_list.types[analyser->returns_list.size++] = type;
}

static inline void assert_returns_type(semantic_analyser_t* analyser, value_t type) {
    if(analyser->returns_list.size == 0) emit_error(analyser, "Expected return value");
    for(size_t i = 0; i < analyser->returns_list.size; ++i) {
        if(analyser->returns_list.types[i].type != type.type) emit_error(analyser, "Invalid return type");
        if(analyser->returns_list.types[i].type == H_VALUE_TYPE) {            
            if(analyser->returns_list.types[i].data_type->type_name->hash != type.data_type->type_name->hash 
                || analyser->returns_list.types[i].data_type->type_name->length != type.data_type->type_name->length 
                || strcmp(analyser->returns_list.types[i].data_type->type_name->string, type.data_type->type_name->string) != 0) {
                    emit_error(analyser, "Invalid return type");
                }
        }
    }
    analyser->returns_list.size = 0;
}

static inline void assert_returns_undefined(semantic_analyser_t* analyser) {
    for(size_t i = 0; i < analyser->returns_list.size; ++i) {
        if(analyser->returns_list.types[i].type != H_VALUE_UNDEFINED) emit_error(analyser, "No return value expected");
    }
    analyser->returns_list.size = 0;
}

static inline void assert_type_undefined(semantic_analyser_t* analyser, h_string_t* type) {
    if(h_ht_types_check_defined(analyser->types_table, type) == 1) emit_error(analyser, "Type already defined");
}

static inline void assert_type_defined(semantic_analyser_t* analyser, h_string_t* type) {
    if(h_ht_types_check_defined(analyser->types_table, type) != 1) emit_error(analyser, "Type undefined");
}

static inline void resolve_returns_list(semantic_analyser_t* analyser, value_t value) {
    if(value.type != H_VALUE_UNDEFINED) {
        assert_returns_type(analyser, value);
        return;
    }
    assert_returns_undefined(analyser);
}

static void emit_error(semantic_analyser_t* analyser, const char* error) {
    DEBUG_LOG("Semantic Analyser Error: %s.\n", error);
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
    
    for(size_t i = 0; i < function->parameters_list_size; ++i) {
        value_t parameter_value = resolve_expression(analyser, parameters_list->block.declarations[i]); 
        if(function->parameters_list_values[i].type != parameter_value.type) emit_error(analyser, "Invalid argument type");
        if(function->parameters_list_values[i].type == H_VALUE_TYPE) {
            if(function->parameters_list_values[i].data_type->type_name->hash != parameter_value.data_type->type_name->hash 
                || function->parameters_list_values[i].data_type->type_name->length != parameter_value.data_type->type_name->length 
                || strcmp(function->parameters_list_values[i].data_type->type_name->string, parameter_value.data_type->type_name->string) != 0) {
                    emit_error(analyser, "Invalid argument type");
                }
        }
    }
}

static inline void assert_loop_count(semantic_analyser_t* analyser) {
    if(analyser->loop_count == 0) emit_error(analyser, "Cannot use statement outside of a loop");
}

static inline void assert_return_count(semantic_analyser_t* analyser) {
    if(analyser->return_count == 0) emit_error(analyser, "Cannot use statement outside of a function");
}

semantic_analyser_t* h_sa_init(ast_node_t** ast_nodes_list, size_t ast_nodes_list_count, h_hash_table_t* globals_table, h_locals_stack_t* locals_stack, h_ht_labels_t* labels_table, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    semantic_analyser_t* analyser = (semantic_analyser_t*)malloc(sizeof(semantic_analyser_t));
    analyser->ast_nodes_list = ast_nodes_list;
    analyser->ast_nodes_list_count = ast_nodes_list_count;
    analyser->globals_table = globals_table;
    analyser->errors_count = 0;
    analyser->scope = 0;
    analyser->loop_count = 0;
    analyser->return_count = 0;
    analyser->locals = locals_stack;
    analyser->initial_locals = locals_stack;
    analyser->labels_table = labels_table;
    analyser->enums_table = enums_table;
    analyser->types_table = types_table;
    analyser->ast_nodes_post_checks_size = 0;
    analyser->ast_nodes_post_checks_capacity = H_DEFAULT_POST_CHECKS_LIST_CAPACITY;
    analyser->ast_nodes_post_checks = (ast_node_t**)malloc(sizeof(ast_node_t*) * analyser->ast_nodes_post_checks_capacity);
    analyser->returns_list.capacity = H_DEFAULT_RETURNS_LIST_CAPACITY;
    analyser->returns_list.size = 0;
    analyser->returns_list.types = (value_t*)malloc(sizeof(value_t) * analyser->returns_list.capacity);
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
            /* if(node->expression.right->type == AST_NODE_ARRAY_INITIALISATION) {
                resolve_expression_array_initialisation(analyser, node->expression.right);
                h_locals_stack_push(analyser->locals, node->expression.left->value.string, node->expression.right->value, analyser->scope);
                h_locals_stack_print(analyser->locals);
                return;
            } */
            value_t rvalue_array = resolve_expression(analyser, node->expression.right);
            assert_value_type(analyser, node->value.type, rvalue_array.array->type);
            h_locals_stack_push(analyser->locals, node->expression.left->value.string, rvalue_array, analyser->scope);
            h_locals_stack_print(analyser->locals);
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
            //h_ht_enums_print(analyser->enums_table);
            h_ht_types_set(analyser->types_table, values->name, NUM_VALUE(0), H_TYPE_INFO_ENUM);
            return;
        case AST_NODE_DECLARATION_FUNCTION:
            ++analyser->return_count;
            h_function_t* function = node->value.function;

            //parameters and return types form functions are saved as .string in parser 
            for(value_t* it = function->parameters_list_values; it != function->parameters_list_values + function->parameters_list_size; ++it) {
                if(it->type == H_VALUE_TYPE) {
                    ht_type_t type = h_ht_types_get(analyser->types_table, it->string);
                    if(type.type == H_TYPE_INFO_UNDEFINED) emit_error(analyser, "Undefined type");
                    h_data_t* data = h_data_init(type.value.data->size);
                    //h_string_free(it->string);
                    data->type_name = type.name; 
                    it->data_type = data;
                }
            }

            if(function->return_type) {
                value_t* it = function->return_type;
                if(it->type == H_VALUE_TYPE) {
                    ht_type_t type = h_ht_types_get(analyser->types_table, it->string);
                    //h_string_free(it->string);
                    if(type.type == H_TYPE_INFO_UNDEFINED) emit_error(analyser, "Undefined type");
                    h_data_t* data = h_data_init(type.value.data->size);
                    data->type_name = type.name; 
                    it->data_type = data;
                }
            }

            h_locals_stack_push(analyser->locals, node->value.function->name, node->value, analyser->scope);
            analyser->locals = node->value.function->locals_stack;
            resolve_block_statement(analyser, node->expression.right);
            analyser->locals = analyser->initial_locals;
            resolve_returns_list(analyser, node->value.function->return_type[0]);
            --analyser->return_count;
            return;
        case AST_NODE_DECLARATION_DATA:
            assert_type_undefined(analyser, node->expression.left->value.string);
            for(size_t i = 0; i < node->value.data->size; ++i) {
                if(node->value.data->field_values[i].type == H_VALUE_TYPE) {
                    assert_type_defined(analyser, node->value.data->field_values[i].string);
                }
            }
            h_ht_types_set(analyser->types_table, node->expression.left->value.string, node->value, H_TYPE_INFO_DATA);
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
    assert_return_count(analyser);
    if(node) {
        value_t return_value = resolve_expression(analyser, node);
        returns_list_push(analyser, return_value);
        return return_value;
    }
    value_t return_value = UNDEFINED_VALUE(0); 
    returns_list_push(analyser, return_value);
    return return_value;
}

static inline value_t resolve_expression_function_call(semantic_analyser_t* analyser, ast_node_t* node) {
    node->expression.left->type = AST_NODE_IDENTIFIER_FUNCTION;
    value_t value_left = resolve_expression(analyser, node->expression.left);
            
    resolve_block_statement(analyser, node->expression.right);
    
    switch(value_left.type) {
        case H_VALUE_FUNCTION:
            assert_parameters_arity(analyser, value_left.function, node->expression.right);
            node->value = *value_left.function->return_type;
            break;
        case H_VALUE_NATIVE:
            node->expression.left->type = AST_NODE_IDENTIFIER_NATIVE;
            //assert_parameters_arity(analyser, value_left.native_fn, node->expression.right);
            node->value.type = value_left.native_fn->return_type->type;
            node->type = AST_NODE_NATIVE_CALL;
            break;
        default:
            emit_error(analyser, "Invalid type for function call");
    }
    
    return node->value;
}

static inline value_t resolve_expression_array_initialisation(semantic_analyser_t* analyser, ast_node_t* node) {
    if(node->block.declarations_size == 0) emit_error(analyser, "Empty array not allowed");
    value_t initial_value = resolve_expression(analyser, node->block.declarations[0]);        
    h_array_t* array = h_array_init(initial_value.type, node->block.declarations_size);
    for(size_t i = 1; i < node->block.declarations_size; ++i) {
        value_t value = resolve_expression(analyser, node->block.declarations[0]);
        assert_value_type(analyser, value.type, initial_value.type);
    }
    array->type = initial_value.type;
    return VALUE_ARRAY(array);
}

static inline value_t resolve_expression_data_initialisation(semantic_analyser_t* analyser, ast_node_t* node) {
    if(node->block.declarations_size == 0) emit_error(analyser, "Empty data not allowed");
    value_t initial_value = resolve_expression(analyser, node->block.declarations[0]);        
    h_data_t* data = h_data_init(node->block.declarations_size);
    h_data_push(data, initial_value);
    for(size_t i = 1; i < node->block.declarations_size; ++i) {
        value_t value = resolve_expression(analyser, node->block.declarations[i]);
        h_data_push(data, value);
    }
    return VALUE_TYPE(data);
}

static inline value_t resolve_expression_dot(semantic_analyser_t* analyser, ast_node_t* node) {
    ast_print(node, 0);
    value_t lvalue = resolve_expression(analyser, node->expression.left);
    if(lvalue.type == H_VALUE_UNDEFINED) emit_error(analyser, "Undefined value");
    assert_value_type(analyser, lvalue.type, H_VALUE_TYPE);

    ht_type_t type = h_ht_types_get(analyser->types_table, lvalue.data_type->type_name);
    if(type.type == H_TYPE_INFO_UNDEFINED) emit_error(analyser, "Undefined type");
    int property_index = h_struct_field_get_index(type.value.data, node->expression.right->value.string);
    if(property_index == -1) {
        emit_error(analyser, "Undefined property");
        return NULL_VALUE();
    }

    node->value = type.value.data->field_values[property_index];
    node->type = AST_NODE_INDEXING;
    node->expression.right->type = AST_NODE_LITERAL;
    node->expression.right->value = NUM_VALUE(property_index);
    //return lvalue.data_type->data[property_index];
    return type.value.data->field_values[property_index];
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
            assert_value_type(analyser, value_left.type, value_right.type);
            break;
        default:
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
        case AST_NODE_IDENTIFIER_TYPE:
            DEBUG_LOG("Identifier type called\n");
            ht_type_t type = h_ht_types_get(analyser->types_table, node->value.string);
            if(type.type == H_TYPE_INFO_UNDEFINED) emit_error(analyser, "Undefined type");

            //this check is only needed if AST_NODE_DATA_INITIALISATION on the right
            //for other scenarios(variable, function call, etc.)the type name being the same is sufficient
            if(node->expression.right->type == AST_NODE_DATA_INITIALISATION) {
                value_t rvalue_data = resolve_expression_data_initialisation(analyser, node->expression.right);
                assert_value_type(analyser, rvalue_data.type, H_VALUE_TYPE);
                rvalue_data.data_type->type_name = type.name;
                if(rvalue_data.data_type->size != type.value.data->size) emit_error(analyser, "Not all field in data populated");
                for(size_t i = 0; i < rvalue_data.data_type->size; ++i) {
                    assert_value_type(analyser, rvalue_data.data_type->data[i].type, type.value.data->field_values[i].type);
                }
                //this is so that the values used for semantic analysis don't get used byt VM
                //otherwise VM would push other values, so size would be doubled
                //ex. {1, 2} -> {1, 2, 1, 2}. This basically resets the pointer to the start of the array
                rvalue_data.data_type->size = 0;
                h_locals_stack_push(analyser->locals, node->expression.left->value.string, rvalue_data, analyser->scope);
                return rvalue_data;
            }
            value_t rvalue_data = resolve_expression(analyser, node->expression.right);
            assert_value_type(analyser, rvalue_data.type, H_VALUE_TYPE);
            if(rvalue_data.data_type->type_name != type.value.data->name) emit_error(analyser, "Not all field in data populated");
            h_locals_stack_push(analyser->locals, node->expression.left->value.string, rvalue_data, analyser->scope);
            return rvalue_data;
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
        case AST_NODE_NATIVE_CALL:
            return resolve_expression_function_call(analyser, node);
        case AST_NODE_DOT:
            return resolve_expression_dot(analyser, node);
        case AST_NODE_ARRAY_INITIALISATION:
            return resolve_expression_array_initialisation(analyser, node);
        case AST_NODE_DATA_INITIALISATION:
            return resolve_expression_data_initialisation(analyser, node);
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
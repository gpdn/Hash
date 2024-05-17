#include "h_icg.h"

static void emit_error(icg_t* icg);
static void icg_generate_declaration_variable(icg_t* icg, ast_node_t* node);
static void icg_generate_declaration_variable_array(icg_t* icg, ast_node_t* node);
static void icg_generate_declaration_variable_global(icg_t* icg, ast_node_t* node);
static inline void icg_generate_declaration_label(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_print(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_if(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_while(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_do_while(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_goto(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_assertion(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_for(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_repeat(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_loop(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_block(icg_t* icg, ast_node_t* node);
static inline void icg_generate_array_initialisation(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_expression(icg_t* icg, ast_node_t* node);
static void icg_generate_expression(icg_t* icg, ast_node_t* node);
/* static inline void icg_generate_number(icg_t* icg, ast_node_t* node);
static inline void icg_generate_string(icg_t* icg, ast_node_t* node); */
static inline void icg_generate_literal(icg_t* icg, ast_node_t* node);
static inline void icg_generate_enum_resolution(icg_t* icg, ast_node_t* node);
static inline void icg_generate_identifier(icg_t* icg, ast_node_t* node);
static inline void icg_generate_identifier_global(icg_t* icg, ast_node_t* node);
static void icg_generate_binary(icg_t* icg, ast_node_t* node);
static void icg_generate_assignment(icg_t* icg, ast_node_t* node);
static void icg_generate_unary(icg_t* icg, ast_node_t* node);
static void icg_generate_post_unary(icg_t* icg, ast_node_t* node);
static inline void icg_generate_indexing(icg_t* icg, ast_node_t* node);
static void gotos_instructions_list_push(goto_instructions_list_t* gotos_list, uint8_t* instruction);

static void emit_error(icg_t* icg) {
    ++icg->errors_count;
    icg->current = icg->ast_nodes_list[icg->ast_nodes_list_count - 1];
}

static void gotos_instructions_list_push(goto_instructions_list_t* gotos_list, uint8_t* instruction) {
    if(gotos_list->size >= gotos_list->capacity) {
        gotos_list->capacity *= 2;
        gotos_list->instructions_list = (uint8_t**)realloc(gotos_list->instructions_list, sizeof(uint8_t*) * gotos_list->capacity);
    }

    gotos_list->instructions_list[gotos_list->size++] = instruction;
    DEBUG_LOG("Pushing Instruction %u\n", *instruction);
}

static void icg_generate_expression(icg_t* icg, ast_node_t* node) {
    switch(node->type) {
        case AST_NODE_DECLARATION_VARIABLE:
        case AST_NODE_DECLARATION_VARIABLE_CONSTANT:
            icg_generate_declaration_variable(icg, node);
            break;
        case AST_NODE_DECLARATION_VARIABLE_ARRAY:
            icg_generate_declaration_variable_array(icg, node);
            break;
        case AST_NODE_DECLARATION_VARIABLE_GLOBAL:
            icg_generate_declaration_variable_global(icg, node);
            break;
        case AST_NODE_DECLARATION_LABEL:
            icg_generate_declaration_label(icg, node);
            break;
        case AST_NODE_DECLARATION_ENUM:
            //icg_generate_declaration_label(icg, node);
            break;
        case AST_NODE_STATEMENT_IF:
            icg_generate_statement_if(icg, node);
            break;
        case AST_NODE_STATEMENT_WHILE:
            icg_generate_statement_while(icg, node);
            break;
        case AST_NODE_STATEMENT_DO_WHILE:
            icg_generate_statement_do_while(icg, node);
            break;
        case AST_NODE_STATEMENT_GOTO:
            icg_generate_statement_goto(icg, node);
            break;
        case AST_NODE_STATEMENT_ASSERTION:
            icg_generate_statement_assertion(icg, node);
            break;
        case AST_NODE_STATEMENT_FOR:
            icg_generate_statement_for(icg, node);
            break;
        case AST_NODE_STATEMENT_REPEAT:
            icg_generate_statement_repeat(icg, node);
            break;
        case AST_NODE_STATEMENT_LOOP:
            icg_generate_statement_loop(icg, node);
            break;
        case AST_NODE_STATEMENT_PRINT:
            icg_generate_statement_print(icg, node);
            break;
        case AST_NODE_STATEMENT_BLOCK:
            icg_generate_statement_block(icg, node);
            break;
        case AST_NODE_STATEMENT_EXPRESSION:
            icg_generate_statement_expression(icg, node);
            break;
        case AST_NODE_LITERAL:
            icg_generate_literal(icg, node);
            break;
        case AST_NODE_ENUM_RESOLUTION:
            icg_generate_enum_resolution(icg, node);
            break;
        case AST_NODE_IDENTIFIER:
            icg_generate_identifier(icg, node);
            break;
        case AST_NODE_IDENTIFIER_GLOBAL:
            icg_generate_identifier_global(icg, node);
            break;
        case AST_NODE_ARRAY_INITIALISATION:
            icg_generate_array_initialisation(icg, node);
            break;
        case AST_NODE_BINARY:
            icg_generate_binary(icg, node);
            break;
        case AST_NODE_INDEXING:
            icg_generate_indexing(icg, node);
            break;
        case AST_NODE_ASSIGNMENT:
            icg_generate_assignment(icg, node);
            break;
        case AST_NODE_UNARY:
            icg_generate_unary(icg, node);
            break;
        case AST_NODE_POST_UNARY:
            icg_generate_post_unary(icg, node);
            break;
        default:
            emit_error(icg);
            break;
    }
}

static void icg_generate_declaration_variable(icg_t* icg, ast_node_t* node) {
    icg_generate_expression(icg, node->expression.right);
    bs_write(icg->bytecode_store, OP_SET_LOCAL);
    bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string));
}

static void icg_generate_declaration_variable_array(icg_t* icg, ast_node_t* node) {
    size_t current_index = icg->bytecode_store->constants->size;
    if(node->expression.right->type == AST_NODE_ARRAY_INITIALISATION) {
        icg_generate_expression(icg, node->expression.right);
        bs_write_constant(icg->bytecode_store, NUM_VALUE(icg->bytecode_store->constants->size - current_index));
        bs_write(icg->bytecode_store, OP_SET_LOCAL_ARRAY);
        bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string));
        return;
    }
    icg_generate_expression(icg, node->expression.right);
    bs_write(icg->bytecode_store, OP_SET_LOCAL);
    bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string));
}

static void icg_generate_declaration_variable_global(icg_t* icg, ast_node_t* node) {
    icg_generate_expression(icg, node->expression.right);
    //bs_write_constant(icg->bytecode_store, node->left->value);
    //bs_write_constant(icg->bytecode_store, NUM_VALUE(h_ht_get_index(icg->globals_table, node->left->value.string)));
    bs_write(icg->bytecode_store, OP_DEFINE_GLOBAL);
    bs_write(icg->bytecode_store, h_ht_get_index(icg->globals_table, node->expression.left->value.string));
}

static inline void icg_generate_declaration_label(icg_t* icg, ast_node_t* node) {
    h_ht_labels_set(icg->labels_table, node->expression.left->value.string, icg->bytecode_store->size);
}

static inline void icg_generate_statement_print(icg_t* icg, ast_node_t* node) {
    icg_generate_expression(icg, node->expression.left);
    bs_write(icg->bytecode_store, OP_PRINT);
}

static inline void icg_generate_statement_if(icg_t* icg, ast_node_t* node) {
    icg_generate_expression(icg, node->expression.left);
    bs_write(icg->bytecode_store, OP_JUMP_IF_FALSE);
    size_t jump_placeholder = bs_write_get(icg->bytecode_store, OP_JUMP_PLACEHOLDER);
    icg_generate_statement_block(icg, node->expression.right);
    icg->bytecode_store->code[jump_placeholder] = icg->bytecode_store->size;
    if(node->expression.other) {
        bs_write(icg->bytecode_store, OP_JUMP);
        size_t else_jump_placeholder = bs_write_get(icg->bytecode_store, OP_JUMP_PLACEHOLDER);
        icg->bytecode_store->code[jump_placeholder] = icg->bytecode_store->size;
        icg_generate_statement_block(icg, node->expression.other);
        icg->bytecode_store->code[else_jump_placeholder] = icg->bytecode_store->size;
        return;
    }
    icg->bytecode_store->code[jump_placeholder] = icg->bytecode_store->size;
    //disassemble_bytecode_store(icg->bytecode_store, "Test", NULL);
}

static inline void icg_generate_statement_while(icg_t* icg, ast_node_t* node) {
    size_t current_instruction_index = icg->bytecode_store->size;
    icg_generate_expression(icg, node->expression.left);
    bs_write(icg->bytecode_store, OP_JUMP_IF_FALSE);
    size_t jump_placeholder = bs_write_get(icg->bytecode_store, OP_JUMP_PLACEHOLDER);
    icg_generate_statement_block(icg, node->expression.right);
    bs_write(icg->bytecode_store, OP_JUMP);
    bs_write(icg->bytecode_store, current_instruction_index);
    icg->bytecode_store->code[jump_placeholder] = icg->bytecode_store->size;
}

static inline void icg_generate_statement_do_while(icg_t* icg, ast_node_t* node) {
    size_t current_instruction_index = icg->bytecode_store->size;
    icg_generate_statement_block(icg, node->expression.left);
    icg_generate_expression(icg, node->expression.right);
    bs_write(icg->bytecode_store, OP_JUMP_IF_TRUE);
    bs_write(icg->bytecode_store, current_instruction_index);
}

static inline void icg_generate_statement_goto(icg_t* icg, ast_node_t* node) {
    bs_write(icg->bytecode_store, OP_GOTO);
    bs_write(icg->bytecode_store, h_ht_labels_get_index(icg->labels_table, node->expression.left->value.string));
    gotos_instructions_list_push(&icg->gotos_list, icg->bytecode_store->code + icg->bytecode_store->size - 1);
}

static inline void icg_generate_statement_assertion(icg_t* icg, ast_node_t* node) {
    icg_generate_expression(icg, node->expression.left);
    bs_write(icg->bytecode_store, OP_JUMP_IF_TRUE);
    size_t jump_placeholder = bs_write_get(icg->bytecode_store, OP_JUMP_PLACEHOLDER);
    icg_generate_statement_print(icg, node->expression.right);
    bs_write(icg->bytecode_store, OP_STOP);
    icg->bytecode_store->code[jump_placeholder] = icg->bytecode_store->size;
}

static inline void icg_generate_statement_for(icg_t* icg, ast_node_t* node) {
    ast_node_t* condition = node->expression.left;
    size_t current_instruction = icg->bytecode_store->size;
    icg_generate_expression(icg, condition->expression.other);
    bs_write(icg->bytecode_store, OP_JUMP_IF_FALSE);
    size_t jump_placeholder = bs_write_get(icg->bytecode_store, OP_JUMP_PLACEHOLDER);
    icg_generate_statement_block(icg, node->expression.right);
    icg_generate_expression(icg, condition->expression.right);
    bs_write(icg->bytecode_store, OP_POP);
    bs_write(icg->bytecode_store, OP_JUMP);
    bs_write(icg->bytecode_store, current_instruction);
    icg->bytecode_store->code[jump_placeholder] = icg->bytecode_store->size;
}

static inline void icg_generate_statement_repeat(icg_t* icg, ast_node_t* node) {
    size_t temp_index = h_locals_stack_get_index(icg->locals_stack, node->expression.other->value.string);
    size_t jump_index = bs_write_get(icg->bytecode_store, OP_PRE_INCREMENT);
    bs_write(icg->bytecode_store, temp_index);
    icg_generate_statement_block(icg, node->expression.right);
    icg_generate_expression(icg, node->expression.left);
    bs_write(icg->bytecode_store, OP_NOT_EQUAL);
    bs_write(icg->bytecode_store, OP_JUMP_IF_TRUE);
    bs_write(icg->bytecode_store, jump_index);
}

static inline void icg_generate_statement_loop(icg_t* icg, ast_node_t* node) {
    size_t it_index = h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string);
    bs_write(icg->bytecode_store, OP_LOOP);
    bs_write(icg->bytecode_store, it_index);
    size_t jump_placeholder = bs_write_get(icg->bytecode_store, OP_JUMP_PLACEHOLDER);
    icg_generate_statement_block(icg, node->expression.right);
    icg->bytecode_store->code[jump_placeholder] = icg->bytecode_store->size;
}

static inline void icg_generate_statement_block(icg_t* icg, ast_node_t* node) {
    for(size_t i = 0; i < node->block.declarations_size; ++i) {
        icg_generate_expression(icg, node->block.declarations[i]);
    }
}

static inline void icg_generate_array_initialisation(icg_t* icg, ast_node_t* node) {
    for(size_t i = 0; i < node->block.declarations_size; ++i) {
        icg_generate_expression(icg, node->block.declarations[i]);
    }
}

static inline void icg_generate_statement_expression(icg_t* icg, ast_node_t* node) {
    icg_generate_expression(icg, node->expression.left);
    bs_write(icg->bytecode_store, OP_POP);
}

static inline void icg_generate_literal(icg_t* icg, ast_node_t* node) {
    bs_write_constant(icg->bytecode_store, node->value);
}

static inline void icg_generate_enum_resolution(icg_t* icg, ast_node_t* node) {
    ht_enum_t* values = h_ht_enums_get(icg->enums_table, node->expression.left->value.string);
    bs_write_constant(icg->bytecode_store, NUM_VALUE(h_ht_enum_value_get(values, node->expression.right->value.string)));
}

static inline void icg_generate_identifier(icg_t* icg, ast_node_t* node) {
    //DEBUG_LOG("IDENTIFIER\n");
    //bs_write_constant(icg->bytecode_store, NUM_VALUE(h_ht_get_index(icg->globals_table, node->value.string)));
    //bs_write_constant(icg->bytecode_store, node->value);
    //bs_write(icg->bytecode_store, OP_GET_GLOBAL);
    bs_write(icg->bytecode_store, OP_GET_LOCAL);
    bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->value.string));
}

static inline void icg_generate_identifier_global(icg_t* icg, ast_node_t* node) {
    //DEBUG_LOG("IDENTIFIER GLOBAL\n");
    //bs_write_constant(icg->bytecode_store, NUM_VALUE(h_ht_get_index(icg->globals_table, node->value.string)));
    //bs_write_constant(icg->bytecode_store, node->value);
    //bs_write(icg->bytecode_store, OP_GET_GLOBAL);
    //ast_print(node, 0);
    bs_write(icg->bytecode_store, OP_GET_GLOBAL);
    bs_write(icg->bytecode_store, h_ht_get_index(icg->globals_table, node->value.string));
}

/* static inline void icg_generate_identifier(icg_t* icg, ast_node_t* node) {
    DEBUG_LOG("IDENTIFIER\n");
    bs_write_constant(icg->bytecode_store, node->value);
    bs_write(icg->bytecode_store, OP_GET_GLOBAL);
} */

static inline void icg_generate_identifier_assignment(icg_t* icg, ast_node_t* node) {
    //DEBUG_LOG("IDENTIFIER ASSIGNMENT\n");
    bs_write_constant(icg->bytecode_store, node->value);
}

static inline void icg_generate_indexing(icg_t* icg, ast_node_t* node) {
    icg_generate_expression(icg, node->expression.right);
    bs_write(icg->bytecode_store, OP_GET_LOCAL_INDEX);
    bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string));
}

/* static inline void icg_generate_number(icg_t* icg, ast_node_t* node) {
    bs_write_constant(icg->bytecode_store, node->value);
}

static inline void icg_generate_string(icg_t* icg, ast_node_t* node) {
    bs_write_constant(icg->bytecode_store, node->value);
} */

static void icg_generate_unary(icg_t* icg, ast_node_t* node) {
    switch(node->operator->type) {
        case H_TOKEN_MINUS:
            icg_generate_expression(icg, node->expression.left);
            bs_write(icg->bytecode_store, OP_NEGATE);
            break;
        case H_TOKEN_BITWISE_NOT:
            icg_generate_expression(icg, node->expression.left);
            bs_write(icg->bytecode_store, OP_BITWISE_NOT);
            break;
        case H_TOKEN_PLUS_PLUS:
            bs_write(icg->bytecode_store, OP_PRE_INCREMENT);
            bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string));
            break;
        case H_TOKEN_MINUS_MINUS:
            bs_write(icg->bytecode_store, OP_PRE_DECREMENT);
            bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string));
            break;
        default:
            return;
    }
}

static void icg_generate_post_unary(icg_t* icg, ast_node_t* node) {
    switch(node->operator->type) {
        case H_TOKEN_PLUS_PLUS:
            bs_write(icg->bytecode_store, OP_POST_INCREMENT);
            bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string));
            break;
        case H_TOKEN_MINUS_MINUS:
            bs_write(icg->bytecode_store, OP_POST_DECREMENT);
            bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string));
            break;
        default:
            return;
    }
}

static void icg_generate_binary(icg_t* icg, ast_node_t* node) {
    
    icg_generate_expression(icg, node->expression.left);
    icg_generate_expression(icg, node->expression.right);

    switch(node->operator->type) {
        case H_TOKEN_PLUS:
            bs_write(icg->bytecode_store, OP_ADD);
            break;
        case H_TOKEN_MINUS:
            bs_write(icg->bytecode_store, OP_SUB);
            break;
        case H_TOKEN_STAR:
            bs_write(icg->bytecode_store, OP_MUL);
            break;
        case H_TOKEN_SLASH:
            bs_write(icg->bytecode_store, OP_DIV);
            break;
        case H_TOKEN_BITWISE_SHIFT_LEFT:
            bs_write(icg->bytecode_store, OP_SHIFT_LEFT);
            break;
        case H_TOKEN_BITWISE_SHIFT_RIGHT:
            bs_write(icg->bytecode_store, OP_SHIFT_RIGHT);
            break;
        case H_TOKEN_BITWISE_AND:
            bs_write(icg->bytecode_store, OP_BITWISE_AND);
            break;
        case H_TOKEN_BITWISE_OR:
            bs_write(icg->bytecode_store, OP_BITWISE_OR);
            break;
        case H_TOKEN_BITWISE_XOR:
            bs_write(icg->bytecode_store, OP_BITWISE_XOR);
            break;
        case H_TOKEN_BITWISE_NOT:
            bs_write(icg->bytecode_store, OP_BITWISE_NOT);
            break;
        case H_TOKEN_DOUBLE_EQUAL:
            bs_write(icg->bytecode_store, OP_EQUALITY);
            break;
        case H_TOKEN_BANG_EQUAL:
            bs_write(icg->bytecode_store, OP_NOT_EQUAL);
            break;
        case H_TOKEN_GREATER:
            bs_write(icg->bytecode_store, OP_GREATER);
            break;
        case H_TOKEN_GREATER_EQUAL:
            bs_write(icg->bytecode_store, OP_GREATER_EQUAL);
            break;
        case H_TOKEN_LESS:
            bs_write(icg->bytecode_store, OP_LESS);
            break;
        case H_TOKEN_LESS_EQUAL:
            bs_write(icg->bytecode_store, OP_LESS_EQUAL);
            break;
        default:
            return;
    }
}

static void icg_generate_assignment(icg_t* icg, ast_node_t* node) {
    icg_generate_expression(icg, node->expression.right);
    //icg_generate_identifier_assignment(icg, node->expression.left);
    //bs_write(icg->bytecode_store, OP_ASSIGN);
    bs_write(icg->bytecode_store, OP_SET_LOCAL);
    bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string));
}

icg_t* icg_init(ast_node_t** ast_nodes_list, size_t ast_nodes_list_count, h_hash_table_t* globals_table, h_locals_stack_t* locals_stack, h_ht_labels_t* labels_table, h_ht_enums_t* enums_table) {
    icg_t* icg = (icg_t*)malloc(sizeof(icg_t));
    bytecode_store_t* bytecode_store = bs_init(ast_nodes_list_count);
    icg->bytecode_store = bytecode_store;
    icg->ast_nodes_list = ast_nodes_list;
    icg->ast_nodes_list_it = ast_nodes_list;
    icg->globals_table = globals_table;
    icg->locals_stack = locals_stack;
    icg->labels_table = labels_table;
    icg->enums_table = enums_table;
    icg->gotos_list.capacity = 50;
    icg->gotos_list.size = 0;
    icg->gotos_list.instructions_list = (uint8_t**)malloc(sizeof(uint8_t*) * icg->gotos_list.capacity);
    return icg;
}

void icg_free(icg_t* icg) {
    free(icg->gotos_list.instructions_list);
    free(icg);
}

void assert_node_type(icg_t* icg, ast_node_type_t type) {
    if(icg->current->type == type) {
        ++icg->current;
        return;
    }
    ++icg->errors_count;
    
    icg->current = icg->ast_nodes_list[icg->ast_nodes_list_count - 1];
}

bytecode_store_t* icg_generate_bytecode(icg_t* icg) {
    icg->current = *icg->ast_nodes_list_it++;
    while(icg->current->type != AST_NODE_EOF) {
        icg_generate_expression(icg, icg->current);
        icg->current = *icg->ast_nodes_list_it++;
    }

    bs_write(icg->bytecode_store, OP_RETURN);

    for(size_t i = 0; i < icg->gotos_list.size; ++i) {
        *icg->gotos_list.instructions_list[i] = h_ht_labels_array_get(icg->labels_table, *icg->gotos_list.instructions_list[i]);
        DEBUG_LOG("%lld - %u\n", i, *icg->gotos_list.instructions_list[i]);
    }

    return icg->bytecode_store;
}
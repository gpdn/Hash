#include "h_icg.h"

static void emit_error(icg_t* icg);
static void icg_generate_declaration_variable(icg_t* icg, ast_node_t* node);
static void icg_generate_declaration_variable_array(icg_t* icg, ast_node_t* node);
static void icg_generate_declaration_data(icg_t* icg, ast_node_t* node);
static void icg_generate_declaration_variable_global(icg_t* icg, ast_node_t* node);
static inline void icg_generate_declaration_label(icg_t* icg, ast_node_t* node);
static void icg_generate_declaration_function(icg_t* icg, ast_node_t* node);
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
static inline void icg_generate_statement_break(icg_t* icg);
static inline void icg_generate_statement_skip(icg_t* icg);
static inline void icg_generate_array_initialisation(icg_t* icg, ast_node_t* node);
static inline void icg_generate_data_initialisation(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_expression(icg_t* icg, ast_node_t* node);
static void icg_generate_expression(icg_t* icg, ast_node_t* node);
/* static inline void icg_generate_number(icg_t* icg, ast_node_t* node);
static inline void icg_generate_string(icg_t* icg, ast_node_t* node); */
static inline void icg_generate_literal(icg_t* icg, ast_node_t* node);
static inline void icg_generate_enum_resolution(icg_t* icg, ast_node_t* node);
static inline void icg_generate_identifier(icg_t* icg, ast_node_t* node);
static inline void icg_generate_identifier_function(icg_t* icg, ast_node_t* node);
static inline void icg_generate_identifier_global(icg_t* icg, ast_node_t* node);
static void icg_generate_binary(icg_t* icg, ast_node_t* node);
static void icg_generate_assignment(icg_t* icg, ast_node_t* node);
static void icg_generate_assignment_compound(icg_t* icg, ast_node_t* node);
static void icg_generate_unary(icg_t* icg, ast_node_t* node);
static void icg_generate_post_unary(icg_t* icg, ast_node_t* node);
static inline void icg_generate_indexing(icg_t* icg, ast_node_t* node);
static inline void icg_generate_function_call(icg_t* icg, ast_node_t* node);
static void gotos_instructions_list_push(goto_instructions_list_t* gotos_list, uint8_t* instruction);
static void breaks_instructions_list_push(break_instructions_list_t* breaks_list, uint8_t* instruction);
static void skips_instructions_list_push(skip_instructions_list_t* skips_list, uint8_t* instruction);
static inline void icg_resolve_breaks(icg_t* icg);
static inline void icg_resolve_skips(icg_t* icg, size_t instruction_index);

static void emit_error(icg_t* icg) {
    ++icg->errors_count;
    icg->current = icg->ast_nodes_list[icg->ast_nodes_list_count - 1];
}

static inline void icg_resolve_breaks(icg_t* icg) {
    for(size_t i = 0; i < icg->breaks_list.size; ++i) {
        *icg->breaks_list.instructions_list[i] = icg->bytecode_store->size;
    }
    icg->breaks_list.size = 0;
}

static inline void icg_resolve_skips(icg_t* icg, size_t instruction_index) {
    for(size_t i = 0; i < icg->skips_list.size; ++i) {
        *icg->skips_list.instructions_list[i] = instruction_index;
    }
    icg->skips_list.size = 0;
}

static void gotos_instructions_list_push(goto_instructions_list_t* gotos_list, uint8_t* instruction) {
    if(gotos_list->size >= gotos_list->capacity) {
        gotos_list->capacity *= 2;
        gotos_list->instructions_list = (uint8_t**)realloc(gotos_list->instructions_list, sizeof(uint8_t*) * gotos_list->capacity);
    }

    gotos_list->instructions_list[gotos_list->size++] = instruction;
    DEBUG_LOG("Pushing Instruction %u\n", *instruction);
}

static void breaks_instructions_list_push(break_instructions_list_t* breaks_list, uint8_t* instruction) {
    if(breaks_list->size >= breaks_list->capacity) {
        breaks_list->capacity *= 2;
        breaks_list->instructions_list = (uint8_t**)realloc(breaks_list->instructions_list, sizeof(uint8_t*) * breaks_list->capacity);
    }

    breaks_list->instructions_list[breaks_list->size++] = instruction;
    DEBUG_LOG("Pushing Break %u\n", *instruction);
}

static void skips_instructions_list_push(skip_instructions_list_t* skips_list, uint8_t* instruction) {
    if(skips_list->size >= skips_list->capacity) {
        skips_list->capacity *= 2;
        skips_list->instructions_list = (uint8_t**)realloc(skips_list->instructions_list, sizeof(uint8_t*) * skips_list->capacity);
    }

    skips_list->instructions_list[skips_list->size++] = instruction;
    DEBUG_LOG("Pushing Skip %u\n", *instruction);
}

static void icg_generate_expression(icg_t* icg, ast_node_t* node) {
    switch(node->type) {
        case AST_NODE_DECLARATION_VARIABLE:
        case AST_NODE_DECLARATION_VARIABLE_CONSTANT:
            icg_generate_declaration_variable(icg, node);
            break;
        case AST_NODE_IDENTIFIER_TYPE:
            icg_generate_declaration_data(icg, node);
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
        case AST_NODE_DECLARATION_DATA:
            //icg_generate_declaration_label(icg, node);
            break;
        case AST_NODE_DECLARATION_FUNCTION:
            icg_generate_declaration_function(icg, node);
            break;
        case AST_NODE_FUNCTION_CALL:
            icg_generate_function_call(icg, node);
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
        case AST_NODE_STATEMENT_BREAK:
            icg_generate_statement_break(icg);
            break;
        case AST_NODE_STATEMENT_SKIP:
            icg_generate_statement_skip(icg);
            break;
        case AST_NODE_STATEMENT_RETURN:
            bs_write(icg->bytecode_store, OP_RETURN);
            break;
        case AST_NODE_STATEMENT_RETURN_VALUE:
            icg_generate_expression(icg, node->expression.left);
            bs_write(icg->bytecode_store, OP_RETURN_VALUE);
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
        case AST_NODE_IDENTIFIER_FUNCTION:
            icg_generate_identifier_function(icg, node);
            break;
        case AST_NODE_IDENTIFIER_GLOBAL:
            icg_generate_identifier_global(icg, node);
            break;
        case AST_NODE_ARRAY_INITIALISATION:
            icg_generate_array_initialisation(icg, node);
            break;
        case AST_NODE_DATA_INITIALISATION:
            icg_generate_data_initialisation(icg, node);
            break;
        case AST_NODE_BINARY:
        case AST_NODE_TO:
            icg_generate_binary(icg, node);
            break;
        case AST_NODE_INDEXING:
            icg_generate_indexing(icg, node);
            break;
        case AST_NODE_ASSIGNMENT:
            icg_generate_assignment(icg, node);
            break;
        case AST_NODE_ASSIGNMENT_COMPOUND:
            icg_generate_assignment_compound(icg, node);
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
    ast_print(node, 0);
    if(node->expression.right) icg_generate_expression(icg, node->expression.right);
    //bs_write(icg->bytecode_store, OP_SET_LOCAL);
    //bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string));
}

static void icg_generate_declaration_variable_array(icg_t* icg, ast_node_t* node) {
    size_t index = h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string);
    bs_write(icg->bytecode_store, OP_DEFINE_LOCAL_ARRAY);
    bs_write(icg->bytecode_store, index);
    if(node->expression.right->type == AST_NODE_ARRAY_INITIALISATION) {
        //size_t current_index = icg->bytecode_store->constants->size;
        bs_write(icg->bytecode_store, OP_START_ARRAY_INITIALISATION);
        icg_generate_expression(icg, node->expression.right);
        //bs_write_constant(icg->bytecode_store, NUM_VALUE(icg->bytecode_store->constants->size - current_index));
        bs_write(icg->bytecode_store, OP_SET_LOCAL_ARRAY);
        bs_write(icg->bytecode_store, index);
        return;
    }
    icg_generate_expression(icg, node->expression.right);
    bs_write(icg->bytecode_store, OP_SET_LOCAL);
    bs_write(icg->bytecode_store, index);
}

static void icg_generate_declaration_data(icg_t* icg, ast_node_t* node) {
    size_t index = h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string);
    bs_write(icg->bytecode_store, OP_DEFINE_LOCAL_ARRAY);
    bs_write(icg->bytecode_store, index);
    if(node->expression.right->type == AST_NODE_DATA_INITIALISATION) {
        bs_write(icg->bytecode_store, OP_START_ARRAY_INITIALISATION);
        icg_generate_expression(icg, node->expression.right);
        bs_write(icg->bytecode_store, OP_SET_LOCAL_DATA);
        bs_write(icg->bytecode_store, index);
        return;
    }
    icg_generate_expression(icg, node->expression.right);
    bs_write(icg->bytecode_store, OP_SET_LOCAL);
    bs_write(icg->bytecode_store, index);
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

static void icg_generate_declaration_function(icg_t* icg, ast_node_t* node) {
    icg->bytecode_store = node->value.function->store;
    //icg->locals_stack = node->value.function->locals_stack;
    ast_print(node->expression.right, 0);
    icg_generate_statement_block(icg, node->expression.right);
    bs_write(node->value.function->store, OP_RETURN);
    icg->bytecode_store = icg->initial_bytecode_store;
    //print_value(&node->value);
    bs_write(icg->bytecode_store, OP_DEFINE_LOCAL);
    bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string));
    #if DEBUG_ALL
    disassemble_bytecode_store(node->value.function->store, node->value.function->name->string, NULL);
    #endif
    //icg->locals_stack = icg->initial_locals_stack;
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
    icg_resolve_breaks(icg);
    icg_resolve_skips(icg, current_instruction_index);
}

static inline void icg_generate_statement_do_while(icg_t* icg, ast_node_t* node) {
    size_t current_instruction_index = icg->bytecode_store->size;
    icg_generate_statement_block(icg, node->expression.left);
    icg_generate_expression(icg, node->expression.right);
    bs_write(icg->bytecode_store, OP_JUMP_IF_TRUE);
    bs_write(icg->bytecode_store, current_instruction_index);
    icg_resolve_breaks(icg);
    icg_resolve_skips(icg, current_instruction_index);
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
    icg_resolve_skips(icg, icg->bytecode_store->size);
    icg_generate_expression(icg, condition->expression.right);
    bs_write(icg->bytecode_store, OP_POP);
    bs_write(icg->bytecode_store, OP_JUMP);
    bs_write(icg->bytecode_store, current_instruction);
    icg->bytecode_store->code[jump_placeholder] = icg->bytecode_store->size;
    icg_resolve_breaks(icg);
}

static inline void icg_generate_statement_repeat(icg_t* icg, ast_node_t* node) {
    size_t temp_index = h_locals_stack_get_index(icg->locals_stack, node->expression.other->value.string);
    bs_write_constant(icg->bytecode_store, NUM_VALUE(0));
    size_t jump_index = bs_write_get(icg->bytecode_store, OP_PRE_INCREMENT);
    bs_write(icg->bytecode_store, temp_index);
    icg_generate_statement_block(icg, node->expression.right);
    icg_generate_expression(icg, node->expression.left);
    bs_write(icg->bytecode_store, OP_NOT_EQUAL);
    bs_write(icg->bytecode_store, OP_JUMP_IF_TRUE);
    bs_write(icg->bytecode_store, jump_index);
    bs_write(icg->bytecode_store, OP_POP);
    icg_resolve_breaks(icg);
    icg_resolve_skips(icg, jump_index);
}

static inline void icg_generate_statement_loop(icg_t* icg, ast_node_t* node) {
    size_t temp_index = h_locals_stack_get_index(icg->locals_stack, node->expression.other->expression.left->value.string);
    size_t count_index = h_locals_stack_get_index(icg->locals_stack, node->expression.other->expression.right->value.string);
    size_t array_index = h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string);
    bs_write_constant(icg->bytecode_store, NUM_VALUE(0));
    bs_write_constant(icg->bytecode_store, NUM_VALUE(0));
    size_t jump_index = bs_write_get(icg->bytecode_store, OP_GET_LOCAL);
    bs_write_get(icg->bytecode_store, count_index); 
    bs_write(icg->bytecode_store, OP_GET_LOCAL_INDEX);
    bs_write(icg->bytecode_store, array_index);
    bs_write(icg->bytecode_store, OP_SET_LOCAL);
    bs_write(icg->bytecode_store, temp_index);
    icg_generate_statement_block(icg, node->expression.right);
    bs_write(icg->bytecode_store, OP_POP);
    bs_write(icg->bytecode_store, OP_GET_LOCAL_SIZE);
    bs_write(icg->bytecode_store, array_index);
    bs_write(icg->bytecode_store, OP_POST_INCREMENT);
    bs_write(icg->bytecode_store, count_index);
    bs_write(icg->bytecode_store, OP_NOT_EQUAL);
    bs_write(icg->bytecode_store, OP_JUMP_IF_TRUE);
    bs_write(icg->bytecode_store, jump_index);
    bs_write(icg->bytecode_store, OP_POP);
    bs_write(icg->bytecode_store, OP_POP);
    icg_resolve_breaks(icg);
    icg_resolve_skips(icg, jump_index);
}

static inline void icg_generate_statement_block(icg_t* icg, ast_node_t* node) {
    for(size_t i = 0; i < node->block.declarations_size; ++i) {
        icg_generate_expression(icg, node->block.declarations[i]);
    }
}

static inline void icg_generate_statement_break(icg_t* icg) {
    bs_write(icg->bytecode_store, OP_JUMP);
    breaks_instructions_list_push(&icg->breaks_list, icg->bytecode_store->code + icg->bytecode_store->size);
    bs_write(icg->bytecode_store, OP_JUMP_PLACEHOLDER);
}

static inline void icg_generate_statement_skip(icg_t* icg) {
    bs_write(icg->bytecode_store, OP_JUMP);
    skips_instructions_list_push(&icg->skips_list, icg->bytecode_store->code + icg->bytecode_store->size);
    bs_write(icg->bytecode_store, OP_JUMP_PLACEHOLDER);
}

static inline void icg_generate_array_initialisation(icg_t* icg, ast_node_t* node) {
    for(size_t i = 0; i < node->block.declarations_size; ++i) {
        icg_generate_expression(icg, node->block.declarations[i]);
    }
}

static inline void icg_generate_data_initialisation(icg_t* icg, ast_node_t* node) {
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
    bs_write(icg->bytecode_store, OP_GET_LOCAL);
    bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->value.string));
}

static inline void icg_generate_identifier_function(icg_t* icg, ast_node_t* node) {
    bs_write(icg->bytecode_store, OP_GET_LOCAL_FUNCTION);
    bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->initial_locals_stack, node->value.string));
}

static inline void icg_generate_identifier_global(icg_t* icg, ast_node_t* node) {
    bs_write(icg->bytecode_store, OP_GET_GLOBAL);
    bs_write(icg->bytecode_store, h_ht_get_index(icg->globals_table, node->value.string));
}

static inline void icg_generate_identifier_assignment(icg_t* icg, ast_node_t* node) {
    bs_write_constant(icg->bytecode_store, node->value);
}

static inline void icg_generate_indexing(icg_t* icg, ast_node_t* node) {
    icg_generate_expression(icg, node->expression.right);
    bs_write(icg->bytecode_store, OP_GET_LOCAL_INDEX);
    bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string));
}

static inline void icg_generate_function_call(icg_t* icg, ast_node_t* node) {
    size_t i = 0;
    icg_generate_expression(icg, node->expression.left);
    for(; i < node->expression.right->block.declarations_size; ++i) {
        ast_print(node->expression.right->block.declarations[i], 0);
        icg_generate_expression(icg, node->expression.right->block.declarations[i]);
    }
    bs_write(icg->bytecode_store, OP_CALL);
    bs_write(icg->bytecode_store, i);
}

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
        case H_TOKEN_TO:
            bs_write(icg->bytecode_store, OP_GENERATE_INTERVAL);
            break;
        default:
            return;
    }
}

static void icg_generate_assignment(icg_t* icg, ast_node_t* node) {
    icg_generate_expression(icg, node->expression.right);
    if(node->expression.left->type == AST_NODE_INDEXING) {
        icg_generate_expression(icg, node->expression.left->expression.right);
        printf("HERE");
        bs_write(icg->bytecode_store, OP_SET_LOCAL_INDEX);
        printf("HERE");
        bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->expression.left->value.string));
        printf("HERE");
        return;
    }
    /* if(node->expression.left->type == AST_NODE_DOT) {
        icg_generate_expression(icg, node->expression.left->expression.right);
        bs_write(icg->bytecode_store, OP_SET_LOCAL_INDEX);
        bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->expression.left->value.string));
        return;
    } */
    bs_write(icg->bytecode_store, OP_SET_LOCAL);
    bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string));
}

static void icg_generate_assignment_compound(icg_t* icg, ast_node_t* node) {

    #define H_ICG_RESOLVE_COMPOUND_OPERATOR_TYPE(type) \
        switch(type) { \
            case H_TOKEN_PLUS_EQUAL: \
                bs_write(icg->bytecode_store, OP_ADD); \
                break; \
            case H_TOKEN_MINUS_EQUAL: \
                bs_write(icg->bytecode_store, OP_SUB); \
                break; \
            case H_TOKEN_STAR_EQUAL: \
                bs_write(icg->bytecode_store, OP_MUL); \
                break; \
            case H_TOKEN_SLASH_EQUAL: \
                bs_write(icg->bytecode_store, OP_DIV); \
                break; \
            default: \
                break; \
        }

    icg_generate_expression(icg, node->expression.right);
    if(node->expression.left->type == AST_NODE_INDEXING) {
        size_t local_index = h_locals_stack_get_index(icg->locals_stack, node->expression.left->expression.left->value.string);
        icg_generate_expression(icg, node->expression.left->expression.right);
        bs_write(icg->bytecode_store, OP_GET_LOCAL_INDEX);
        bs_write(icg->bytecode_store, local_index);
        H_ICG_RESOLVE_COMPOUND_OPERATOR_TYPE(node->operator->type);
        icg_generate_expression(icg, node->expression.left->expression.right);
        bs_write(icg->bytecode_store, OP_SET_LOCAL_INDEX);
        bs_write(icg->bytecode_store, local_index);
        return;
    }

    size_t local_index = h_locals_stack_get_index(icg->locals_stack, node->expression.left->value.string);
    bs_write(icg->bytecode_store, OP_GET_LOCAL);
    bs_write(icg->bytecode_store, local_index);
    H_ICG_RESOLVE_COMPOUND_OPERATOR_TYPE(node->operator->type);
    bs_write(icg->bytecode_store, OP_SET_LOCAL);
    bs_write(icg->bytecode_store, local_index);

    #undef H_ICG_RESOLVE_COMPOUND_OPERATOR_TYPE
}

icg_t* icg_init(ast_node_t** ast_nodes_list, size_t ast_nodes_list_count, h_hash_table_t* globals_table, h_locals_stack_t* locals_stack, h_ht_labels_t* labels_table, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    icg_t* icg = (icg_t*)malloc(sizeof(icg_t));
    bytecode_store_t* bytecode_store = bs_init(ast_nodes_list_count);
    icg->bytecode_store = bytecode_store;
    icg->initial_bytecode_store = bytecode_store;
    icg->ast_nodes_list = ast_nodes_list;
    icg->ast_nodes_list_it = ast_nodes_list;
    icg->globals_table = globals_table;
    icg->locals_stack = locals_stack;
    icg->initial_locals_stack = locals_stack;
    icg->labels_table = labels_table;
    icg->enums_table = enums_table;
    icg->types_table = types_table;
    icg->gotos_list.capacity = 50;
    icg->gotos_list.size = 0;
    icg->gotos_list.instructions_list = (uint8_t**)malloc(sizeof(uint8_t*) * icg->gotos_list.capacity);
    icg->breaks_list.capacity = 5;
    icg->breaks_list.size = 0;
    icg->breaks_list.instructions_list = (uint8_t**)malloc(sizeof(uint8_t*) * icg->breaks_list.capacity);
    icg->skips_list.capacity = 5;
    icg->skips_list.size = 0;
    icg->skips_list.instructions_list = (uint8_t**)malloc(sizeof(uint8_t*) * icg->skips_list.capacity);
    return icg;
}

void icg_free(icg_t* icg) {
    free(icg->gotos_list.instructions_list);
    free(icg->breaks_list.instructions_list);
    free(icg->skips_list.instructions_list);
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

    bs_write(icg->bytecode_store, OP_STOP);

    for(size_t i = 0; i < icg->gotos_list.size; ++i) {
        *icg->gotos_list.instructions_list[i] = h_ht_labels_array_get(icg->labels_table, *icg->gotos_list.instructions_list[i]);
        DEBUG_LOG("%lld - %u\n", i, *icg->gotos_list.instructions_list[i]);
    }

    return icg->bytecode_store;
}
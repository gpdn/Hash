#include "h_icg.h"

static void emit_error(icg_t* icg);
static void icg_generate_declaration_variable(icg_t* icg, ast_node_t* node);
static void icg_generate_declaration_variable_global(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_print(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_block(icg_t* icg, ast_node_t* node);
static inline void icg_generate_statement_expression(icg_t* icg, ast_node_t* node);
static void icg_generate_expression(icg_t* icg, ast_node_t* node);
/* static inline void icg_generate_number(icg_t* icg, ast_node_t* node);
static inline void icg_generate_string(icg_t* icg, ast_node_t* node); */
static inline void icg_generate_literal(icg_t* icg, ast_node_t* node);
static inline void icg_generate_identifier(icg_t* icg, ast_node_t* node);
static inline void icg_generate_identifier_global(icg_t* icg, ast_node_t* node);
static void icg_generate_binary(icg_t* icg, ast_node_t* node);
static void icg_generate_assignment(icg_t* icg, ast_node_t* node);
static void icg_generate_unary(icg_t* icg, ast_node_t* node);

static void emit_error(icg_t* icg) {
    ++icg->errors_count;
    icg->current = icg->ast_nodes_list[icg->ast_nodes_list_count - 1];
}

static void icg_generate_expression(icg_t* icg, ast_node_t* node) {
    switch(node->type) {
        case AST_NODE_DECLARATION_VARIABLE:
        case AST_NODE_DECLARATION_VARIABLE_CONSTANT:
            icg_generate_declaration_variable(icg, node);
            break;
        case AST_NODE_DECLARATION_VARIABLE_GLOBAL:
            icg_generate_declaration_variable_global(icg, node);
            break;
        case AST_NODE_STATEMENT_PRINT:
            icg_generate_statement_print(icg, node);
            break;
        case AST_NODE_STATEMENT_BLOCK:
            icg_generate_statement_block(icg, node);
            DEBUG_LOG("BLOCK CALLED\n");
            break;
        case AST_NODE_STATEMENT_EXPRESSION:
            icg_generate_statement_expression(icg, node);
            break;
        case AST_NODE_LITERAL:
            /* switch(node->operator->type) {
                case H_TOKEN_NUMBER_LITERAL:
                    icg_generate_number(icg, node);
                    break;
                case H_TOKEN_STRING_LITERAL:
                    icg_generate_string(icg, node);
                    break;
                default:
                    emit_error(icg);
                    break;
            } */
            icg_generate_literal(icg, node);
            break;
        case AST_NODE_IDENTIFIER:
            icg_generate_identifier(icg, node);
            break;
        case AST_NODE_IDENTIFIER_GLOBAL:
            icg_generate_identifier_global(icg, node);
            break;
        case AST_NODE_BINARY:
            icg_generate_binary(icg, node);
            break;
        case AST_NODE_ASSIGNMENT:
            icg_generate_assignment(icg, node);
            break;
        case AST_NODE_UNARY:
            DEBUG_LOG("UNARY CALLED");
            icg_generate_unary(icg, node);
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
    printf("%s", "Here");
}

static void icg_generate_declaration_variable_global(icg_t* icg, ast_node_t* node) {
    icg_generate_expression(icg, node->expression.right);
    //bs_write_constant(icg->bytecode_store, node->left->value);
    //bs_write_constant(icg->bytecode_store, NUM_VALUE(h_ht_get_index(icg->globals_table, node->left->value.string)));
    bs_write(icg->bytecode_store, OP_DEFINE_GLOBAL);
    bs_write(icg->bytecode_store, h_ht_get_index(icg->globals_table, node->expression.left->value.string));
}

static inline void icg_generate_statement_print(icg_t* icg, ast_node_t* node) {
    icg_generate_expression(icg, node->expression.left);
    bs_write(icg->bytecode_store, OP_PRINT);
}

static inline void icg_generate_statement_block(icg_t* icg, ast_node_t* node) {
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

static inline void icg_generate_identifier(icg_t* icg, ast_node_t* node) {
    DEBUG_LOG("IDENTIFIER\n");
    //bs_write_constant(icg->bytecode_store, NUM_VALUE(h_ht_get_index(icg->globals_table, node->value.string)));
    //bs_write_constant(icg->bytecode_store, node->value);
    //bs_write(icg->bytecode_store, OP_GET_GLOBAL);
    bs_write(icg->bytecode_store, OP_GET_LOCAL);
    bs_write(icg->bytecode_store, h_locals_stack_get_index(icg->locals_stack, node->value.string));
}

static inline void icg_generate_identifier_global(icg_t* icg, ast_node_t* node) {
    DEBUG_LOG("IDENTIFIER GLOBAL\n");
    //bs_write_constant(icg->bytecode_store, NUM_VALUE(h_ht_get_index(icg->globals_table, node->value.string)));
    //bs_write_constant(icg->bytecode_store, node->value);
    //bs_write(icg->bytecode_store, OP_GET_GLOBAL);
    ast_print(node, 0);
    bs_write(icg->bytecode_store, OP_GET_GLOBAL);
    bs_write(icg->bytecode_store, h_ht_get_index(icg->globals_table, node->value.string));
}

/* static inline void icg_generate_identifier(icg_t* icg, ast_node_t* node) {
    DEBUG_LOG("IDENTIFIER\n");
    bs_write_constant(icg->bytecode_store, node->value);
    bs_write(icg->bytecode_store, OP_GET_GLOBAL);
} */

static inline void icg_generate_identifier_assignment(icg_t* icg, ast_node_t* node) {
    DEBUG_LOG("IDENTIFIER ASSIGNMENT\n");
    bs_write_constant(icg->bytecode_store, node->value);
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
            icg_generate_identifier_assignment(icg, node->expression.left);
            bs_write(icg->bytecode_store, OP_PRE_INCREMENT);
            break;
        case H_TOKEN_MINUS_MINUS:
            icg_generate_identifier_assignment(icg, node->expression.left);
            bs_write(icg->bytecode_store, OP_PRE_DECREMENT);
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
    icg_generate_identifier_assignment(icg, node->expression.left);
    bs_write(icg->bytecode_store, OP_ASSIGN);
}

icg_t* icg_init(ast_node_t** ast_nodes_list, size_t ast_nodes_list_count, h_hash_table_t* globals_table, h_locals_stack_t* locals_stack) {
    icg_t* icg = (icg_t*)malloc(sizeof(icg_t));
    bytecode_store_t* bytecode_store = bs_init(ast_nodes_list_count);
    icg->bytecode_store = bytecode_store;
    icg->ast_nodes_list = ast_nodes_list;
    icg->ast_nodes_list_it = ast_nodes_list;
    icg->globals_table = globals_table;
    icg->locals_stack = locals_stack;
    return icg;
}

void icg_free(icg_t* icg) {
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

    return icg->bytecode_store;
}
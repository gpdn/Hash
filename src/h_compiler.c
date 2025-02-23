#include "h_compiler.h"

#define H_COMPILER_BUFFER_INITIAL_CAPACITY 100

void ast_compile_node(h_compiler_t* compiler, ast_node_t* current);
void ast_compile_expression(h_compiler_t* compiler, ast_node_t* current);
h_string_t compiler_buffer_init();
static inline void ast_compile_type_token(h_compiler_t* compiler, token_type_t type);
static inline void ast_compile_type_value(h_compiler_t* compiler, value_t* value);
static inline void ast_compile_declaration_variable(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_declaration_variable_no_newline(h_compiler_t* compiler, ast_node_t* current);
static void ast_compile_declaration_data(h_compiler_t* compiler, ast_node_t* current);
static void ast_compile_declaration_enum(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_declaration_function(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_declaration_label(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_statement_print(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_statement_return(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_statement_return_value(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_statement_skip(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_statement_break(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_statement_if(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_statement_while(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_statement_do_while(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_statement_for(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_statement_goto(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_statement_expression(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_statement_block(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_statement_block_enum(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_identifier(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_expression_binary(h_compiler_t* compiler, ast_node_t* current);
static inline void ast_compile_expression_unary(h_compiler_t* compiler, ast_node_t* current);

static inline void ast_compile_type_token(h_compiler_t* compiler, token_type_t type) {
    switch(type) {
        case H_TOKEN_NUM:
            h_string_append_cstring(compiler->current_buffer, "int ");
            break;
        case H_TOKEN_STR:
            h_string_append_cstring(compiler->current_buffer, "char* ");
            break;
        default:
            DEBUG_LOG("Unrecognised type in ast_compile_type at line %d\n", __LINE__);
            break;
    }
}

static inline void ast_compile_type_value(h_compiler_t* compiler, value_t* value) {
    switch(value->type) {
        case H_VALUE_NUMBER:
            h_string_append_cstring(compiler->current_buffer, "int ");
            break;
        case H_VALUE_STRING:
            h_string_append_cstring(compiler->current_buffer, "char* ");
            break;
        default:
            DEBUG_LOG("Unrecognised type in ast_compile_type at line %d\n", __LINE__);
            break;
    }
}

static inline void ast_compile_identifier(h_compiler_t* compiler, ast_node_t* current) {
    h_string_append_cstring_n(compiler->current_buffer, current->operator->start, current->operator->length);
}

static inline void ast_compile_declaration_variable(h_compiler_t* compiler, ast_node_t* current) {
    ast_compile_type_token(compiler, current->operator->type);
    ast_compile_identifier(compiler, current->expression.left);
    if(!current->expression.left) return;
    h_string_append_cstring(compiler->current_buffer, " = ");
    ast_compile_expression(compiler, current->expression.right);
    h_string_append_cstring(compiler->current_buffer, ";\n");
}

static inline void ast_compile_declaration_variable_no_newline(h_compiler_t* compiler, ast_node_t* current) {
    ast_compile_type_token(compiler, current->operator->type);
    ast_compile_identifier(compiler, current->expression.left);
    if(!current->expression.left) return;
    h_string_append_cstring(compiler->current_buffer, " = ");
    ast_compile_expression(compiler, current->expression.right);
}

static void ast_compile_declaration_data(h_compiler_t* compiler, ast_node_t* current) {
    h_string_t* current_buffer = compiler->current_buffer;
    compiler->current_buffer = &compiler->h_buffer;
    h_string_append_cstring(compiler->current_buffer, "typedef struct ");
    ht_type_t type = h_ht_types_get(compiler->types_table, current->expression.left->value.string);
    h_string_append(compiler->current_buffer, type.name);
    h_string_append_cstring(compiler->current_buffer, " {\n");
    for(size_t i = 0; i < type.value.data->size; ++i) {
        h_string_append_cstring(compiler->current_buffer, "\t");
        ast_compile_type_value(compiler, type.value.data->field_values + i);
        h_string_append(compiler->current_buffer, type.value.data->field_names + i);
        h_string_append_cstring(compiler->current_buffer, ";\n");
    }
    h_string_append_cstring(compiler->current_buffer, "} ");
    h_string_append(compiler->current_buffer, type.name);
    h_string_append_cstring(compiler->current_buffer, ";\n\n");
    compiler->current_buffer = current_buffer;
}

static void ast_compile_declaration_enum(h_compiler_t* compiler, ast_node_t* current) {
    h_string_t* current_buffer = compiler->current_buffer;
    compiler->current_buffer = &compiler->h_buffer;
    h_string_append_cstring(compiler->current_buffer, "typedef enum ");
    h_string_append(compiler->current_buffer, current->expression.left->value.string);
    h_string_append_cstring(compiler->current_buffer, " {\n");
    ast_compile_statement_block_enum(compiler, current->expression.right);
    h_string_append_cstring(compiler->current_buffer, "} ");
    h_string_append(compiler->current_buffer, current->expression.left->value.string);
    h_string_append_cstring(compiler->current_buffer, ";\n\n");
    compiler->current_buffer = current_buffer;
}

static void ast_compile_function_signature(h_compiler_t* compiler, h_function_t* function, const char* termination) {
    ast_compile_type_value(compiler, &function->return_type[0]);
    h_string_append(compiler->current_buffer, function->name);
    h_string_append_cstring(compiler->current_buffer, "(");
    size_t i = 0;
    for(; i < function->parameters_list_size - 1; ++i) {
        ast_compile_type_value(compiler, function->parameters_list_values + i);
        h_string_append(compiler->current_buffer, function->parameters_list_names[i]);
        h_string_append_cstring(compiler->current_buffer, ", ");
    }
    ast_compile_type_value(compiler, function->parameters_list_values + i);
    h_string_append(compiler->current_buffer, function->parameters_list_names[i]);
    h_string_append_cstring(compiler->current_buffer, ")");
    h_string_append_cstring(compiler->current_buffer, termination);
}

static inline void ast_compile_declaration_function(h_compiler_t* compiler, ast_node_t* current) {
    h_string_t* current_buffer = compiler->current_buffer;
    compiler->current_buffer = &compiler->h_buffer;
    h_function_t* function = current->value.function;
    ast_compile_function_signature(compiler, function, ";\n");
    compiler->current_buffer = &compiler->c_buffer;
    ast_compile_function_signature(compiler, function, " {\n");
    ast_compile_statement_block(compiler, current->expression.right);
    h_string_append_cstring(compiler->current_buffer, "}\n\n");
    compiler->current_buffer = current_buffer;
}

static inline void ast_compile_declaration_label(h_compiler_t* compiler, ast_node_t* current) {
    h_string_append_cstring(compiler->current_buffer, "\n");
    ast_compile_expression(compiler, current->expression.left);
    h_string_append_cstring(compiler->current_buffer, ":\n");
}


static inline void ast_compile_statement_print(h_compiler_t* compiler, ast_node_t* current) {
    h_string_append_cstring(compiler->current_buffer, "printf(");
    switch(current->expression.left->type) {
        case H_VALUE_NUMBER:
            h_string_append_cstring(compiler->current_buffer, "\"\%d\"");
            break;
        case H_VALUE_STRING:
            h_string_append_cstring(compiler->current_buffer, "\"\%s\"");
            break;
        default:
            DEBUG_LOG("Unimplemented node in ast_compile_statement_print at line %d\n", __LINE__);
            break;
    }
    h_string_append_cstring(compiler->current_buffer, ", ");
    ast_compile_expression(compiler, current->expression.left);
    h_string_append_cstring(compiler->current_buffer, ");\n");
}

static inline void ast_compile_statement_return(h_compiler_t* compiler, ast_node_t* current) {
    h_string_append_cstring(compiler->current_buffer, "return ");
    ast_compile_expression(compiler, current->expression.left);
    h_string_append_cstring(compiler->current_buffer, ";\n");
}

static inline void ast_compile_statement_skip(h_compiler_t* compiler, ast_node_t* current) {
    h_string_append_cstring(compiler->current_buffer, "return;\n");
}

static inline void ast_compile_statement_break(h_compiler_t* compiler, ast_node_t* current) {
    h_string_append_cstring(compiler->current_buffer, "break;\n");
}

static inline void ast_compile_statement_if(h_compiler_t* compiler, ast_node_t* current) {
    h_string_append_cstring(compiler->current_buffer, "\nif(");
    ast_compile_expression(compiler, current->expression.left);
    h_string_append_cstring(compiler->current_buffer, ") {\n");
    ast_compile_statement_block(compiler, current->expression.right);
    if(current->expression.other) {
        h_string_append_cstring(compiler->current_buffer, "} else {\n");
        ast_compile_statement_block(compiler, current->expression.right);
        h_string_append_cstring(compiler->current_buffer, "}\n");
        return;
    }
    h_string_append_cstring(compiler->current_buffer, "}\n");
}

static inline void ast_compile_statement_while(h_compiler_t* compiler, ast_node_t* current) {
    h_string_append_cstring(compiler->current_buffer, "\nwhile(");
    ast_compile_expression(compiler, current->expression.left);
    h_string_append_cstring(compiler->current_buffer, ") {\n");
    ast_compile_statement_block(compiler, current->expression.right);
    h_string_append_cstring(compiler->current_buffer, "}\n");
}

static inline void ast_compile_statement_do_while(h_compiler_t* compiler, ast_node_t* current) {
    h_string_append_cstring(compiler->current_buffer, "\ndo {\n");
    ast_compile_statement_block(compiler, current->expression.left);
    h_string_append_cstring(compiler->current_buffer, "} while(");
    ast_compile_expression(compiler, current->expression.right);
    h_string_append_cstring(compiler->current_buffer, ");\n");
}

static inline void ast_compile_statement_for(h_compiler_t* compiler, ast_node_t* current) {
    h_string_append_cstring(compiler->current_buffer, "\nfor(");
    ast_node_t* condition = current->expression.left;
    if(condition->expression.left) ast_compile_declaration_variable_no_newline(compiler, condition->expression.left);
    h_string_append_cstring(compiler->current_buffer, "; ");
    ast_compile_expression(compiler, condition->expression.other);
    h_string_append_cstring(compiler->current_buffer, "; ");
    ast_compile_expression(compiler, condition->expression.right);
    h_string_append_cstring(compiler->current_buffer, ") {\n");
    ast_compile_statement_block(compiler, current->expression.right);
    h_string_append_cstring(compiler->current_buffer, "}\n");
}

static inline void ast_compile_statement_goto(h_compiler_t* compiler, ast_node_t* current) {
    h_string_append_cstring(compiler->current_buffer, "goto ");
    ast_compile_identifier(compiler, current->expression.left);
    h_string_append_cstring(compiler->current_buffer, ";\n");
}

static inline void ast_compile_statement_return_value(h_compiler_t* compiler, ast_node_t* current) {
    h_string_append_cstring(compiler->current_buffer, "return;\n");
}

static inline void ast_compile_statement_expression(h_compiler_t* compiler, ast_node_t* current) {
    ast_compile_expression(compiler, current->expression.left);
    h_string_append_cstring(compiler->current_buffer, ";\n");
}


static inline void ast_compile_statement_block(h_compiler_t* compiler, ast_node_t* current) {
    for(size_t i = 0; i < current->block.declarations_size; ++i) {
        ast_compile_node(compiler, current->block.declarations[i]);
    }
}

static inline void ast_compile_statement_block_enum(h_compiler_t* compiler, ast_node_t* current) {
    for(size_t i = 0; i < current->block.declarations_size; ++i) {
        h_string_append_cstring(compiler->current_buffer, "\t");
        ast_compile_identifier(compiler, current->block.declarations[i]);
        h_string_append_cstring(compiler->current_buffer, ",\n");
    }
}

inline void ast_compile_expression_binary(h_compiler_t* compiler, ast_node_t* current) {
    ast_compile_expression(compiler, current->expression.left);
    h_string_append_cstring(compiler->current_buffer, " ");
    h_string_append_cstring_n(compiler->current_buffer, current->operator->start, current->operator->length);
    h_string_append_cstring(compiler->current_buffer, " ");
    ast_compile_expression(compiler, current->expression.right);
}

inline void ast_compile_expression_unary(h_compiler_t* compiler, ast_node_t* current) {
    h_string_append_cstring_n(compiler->current_buffer, current->operator->start, current->operator->length);
    ast_compile_expression(compiler, current->expression.left);
}

void ast_compile_expression(h_compiler_t* compiler, ast_node_t* current) {
    switch(current->type) {
        case AST_NODE_LITERAL:
            h_string_append_cstring_n(compiler->current_buffer, current->operator->start, current->operator->length);
            break;
        case AST_NODE_IDENTIFIER:
            ast_compile_identifier(compiler, current);    
            break;
        case AST_NODE_BINARY:
        case AST_NODE_ASSIGNMENT:
        case AST_NODE_ASSIGNMENT_COMPOUND:
            ast_compile_expression_binary(compiler, current);
            break;
        case AST_NODE_UNARY:
            ast_compile_expression_unary(compiler, current);
            break;
        default:
            DEBUG_LOG("Unimplemented node in ast compile expression at line %d\n", __LINE__);
            break;
    }
}

void ast_compile_node(h_compiler_t* compiler, ast_node_t* current) {
    switch(current->type) {
        case AST_NODE_DECLARATION_VARIABLE:
            ast_compile_declaration_variable(compiler, current);
            return;
        case AST_NODE_DECLARATION_DATA:
            ast_compile_declaration_data(compiler, current);
            break;
        case AST_NODE_DECLARATION_ENUM:
            ast_compile_declaration_enum(compiler, current);
            break;
        case AST_NODE_DECLARATION_FUNCTION:
            ast_compile_declaration_function(compiler, current);
            break;
        case AST_NODE_DECLARATION_LABEL:
            ast_compile_declaration_label(compiler, current);
            break;
        case AST_NODE_STATEMENT_BLOCK:
            ast_compile_statement_block(compiler, current);
            break;
        case AST_NODE_STATEMENT_PRINT:
            ast_compile_statement_print(compiler, current);
            break;
        case AST_NODE_STATEMENT_RETURN:
            ast_compile_statement_return(compiler, current);
            break;
        case AST_NODE_STATEMENT_RETURN_VALUE:
            ast_compile_statement_return_value(compiler, current);
            break;
        case AST_NODE_STATEMENT_IF:
            ast_compile_statement_if(compiler, current);
            break;
        case AST_NODE_STATEMENT_WHILE:
            ast_compile_statement_while(compiler, current);
            break;
        case AST_NODE_STATEMENT_DO_WHILE:
            ast_compile_statement_do_while(compiler, current);
            break;
        case AST_NODE_STATEMENT_FOR:
            ast_compile_statement_for(compiler, current);
            break;
        case AST_NODE_STATEMENT_GOTO:
            ast_compile_statement_goto(compiler, current);
            break;
        case AST_NODE_STATEMENT_EXPRESSION:
            ast_compile_statement_expression(compiler, current);
            break;
        default:
            DEBUG_LOG("Unimplemented node in ast compile node at line %d\n", __LINE__);
    }
}

h_string_t compiler_buffer_init() {
    h_string_t h_string;
    h_string.string = (char*)malloc(sizeof(char) * H_COMPILER_BUFFER_INITIAL_CAPACITY);
    h_string.length = 0;
    h_string.capacity = H_COMPILER_BUFFER_INITIAL_CAPACITY;
    h_string.hash = 0;
    return h_string;
}

h_compiler_t h_compiler_init(ast_node_t** ast_nodes_list, size_t ast_nodes_list_count, h_ht_types_t* types_table, h_ht_enums_t* enums_table) {
    h_compiler_t compiler = {
        .ast_nodes_list = ast_nodes_list,
        .ast_nodes_list_count = ast_nodes_list_count,
        .current = NULL,
        .main_buffer = compiler_buffer_init(),
        .c_buffer = compiler_buffer_init(),
        .h_buffer = compiler_buffer_init(),
        .types_table = types_table,
        .enums_table = enums_table
    };
    return compiler;
}

void h_compiler_run(h_compiler_t* compiler) {
    h_string_append_cstring(&compiler->h_buffer, "#ifndef H_COMPILED\n#define H_COMPILED\n\n");
    h_string_append_cstring(&compiler->c_buffer, "#include \"compiled.h\"\n\n");
    h_string_append_cstring(&compiler->main_buffer, "int main() {\n");
    compiler->current_buffer = &compiler->main_buffer;
    compiler->current = *compiler->ast_nodes_list++;
    while(compiler->current->type != AST_NODE_EOF) {
        ast_compile_node(compiler, compiler->current);
        compiler->current = *compiler->ast_nodes_list++;
    }
    DEBUG_LOG("h: %.*s\n", (int)compiler->h_buffer.length, compiler->h_buffer.string);
    DEBUG_LOG("c: %.*s\n", (int)compiler->c_buffer.length, compiler->c_buffer.string);
    h_string_append_cstring(&compiler->h_buffer, "\n#endif");
    h_string_append_cstring(&compiler->main_buffer, "\n\treturn 0;\n}");
    write_file("compiled.h", compiler->h_buffer.string, sizeof(char), compiler->h_buffer.length);
    write_file("compiled.c", compiler->c_buffer.string, sizeof(char), compiler->c_buffer.length);
    append_file_str("compiled.c", compiler->main_buffer.string, sizeof(char), compiler->main_buffer.length);
}

void h_compiler_free(h_compiler_t* compiler) {
    free((void*)compiler->main_buffer.string);
    free((void*)compiler->c_buffer.string);
    free((void*)compiler->h_buffer.string);
}

#undef H_COMPILER_BUFFER_INITIAL_CAPACITY
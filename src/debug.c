

#include "debug.h"

#if DEBUG

static int basic_instruction(const char* name, size_t offset, FILE* file);
static int constant_instruction(const char* name, bytecode_store_t* store, size_t offset, FILE* file);
static inline void bs_print_value(bytecode_store_t* store, size_t offset, value_t value);
const char* resolve_token_type(token_type_t type);

static int basic_instruction(const char* name, size_t offset, FILE* file) {
    DEBUG_LOG("%s\n", name);
    if(file) {
        char* string = (char*)malloc((sizeof(char) * (strlen(name) + 5)));
        sprintf(string, "%s\n", name);
        fwrite(string, 1, strlen(string), file);
        free((void*)string);
    }
    return offset + 1;
}

static inline void bs_print_value(bytecode_store_t* store, size_t offset, value_t value) {
    switch(value.type) {
        case H_VALUE_NUMBER:
            DEBUG_LOG("%0.2f\n", store->constants->constants[store->code[offset + 1]].number);
            break;
        case H_VALUE_STRING:
            DEBUG_LOG("%s\n", store->constants->constants[store->code[offset + 1]].string->string);
            break;
        default: 
            break;
    }
} 

static int constant_instruction(const char* name, bytecode_store_t* store, size_t offset, FILE* file) {
    DEBUG_LOG("%s ", name);
    value_t value = store->constants->constants[store->code[offset + 1]];
    #if DEBUG_ALL
        bs_print_value(store, offset, value);
    #endif
    if(file) {
        char* string = (char*)malloc((sizeof(char) * (strlen(name) + 50)));
        sprintf(string, "%s %0.2f\n", name, store->constants->constants[store->code[offset + 1]].number);
        fwrite(string, 1, strlen(string), file);
        free((void*)string);
    }
    return offset + 2;
}

void disassemble_bytecode_store(bytecode_store_t* store, const char* name, FILE* file) {
    
    DEBUG_COLOR_SET(COLOR_CYAN);
    DEBUG_TITLE(name);

    for(size_t offset = 0; offset < store->size;) {
        offset = disassemble_instruction(store, offset, file);
    }
    DEBUG_COLOR_RESET();
}

size_t disassemble_instruction(bytecode_store_t* store, size_t offset, FILE* file) {
    printf("%04lld  ", offset);

    uint8_t instruction = store->code[offset];
    switch(instruction) {
        case OP_RETURN:
            return basic_instruction("OP_RETURN", offset, file);
            break;
        case OP_START:
            return basic_instruction("OP_START", offset, file);
            break;
        case OP_STOP:
            return basic_instruction("OP_STOP", offset, file);
            break;
        case OP_NEGATE:
            return basic_instruction("OP_NEGATE", offset, file);
            break;
        case OP_ADD:
            return basic_instruction("OP_ADD", offset, file);
            break;
        case OP_SUB:
            return basic_instruction("OP_SUB", offset, file);
            break;
        case OP_MUL:
            return basic_instruction("OP_MUL", offset, file);
            break;
        case OP_DIV:
            return basic_instruction("OP_DIV", offset, file);
            break;
        case OP_CONSTANT:
            return constant_instruction("OP_CONSTANT", store, offset, file);
            break;
        case OP_SHIFT_LEFT:
            return basic_instruction("OP_SHIFT_LEFT", offset, file);
            break;
        case OP_SHIFT_RIGHT:
            return basic_instruction("OP_SHIFT_RIGHT", offset, file);
            break;
        case OP_BITWISE_AND:
            return basic_instruction("OP_BITWISE_AND", offset, file);
            break;
        case OP_BITWISE_OR:
            return basic_instruction("OP_BITWISE_OR", offset, file);
            break;
        case OP_BITWISE_XOR:
            return basic_instruction("OP_BITWISE_XOR", offset, file);
            break;
        case OP_BITWISE_NOT:
            return basic_instruction("OP_BITWISE_NOT", offset, file);
            break;
        case OP_EQUALITY:
            return basic_instruction("OP_EQUALITY", offset, file);
            break;
        case OP_NOT_EQUAL:
            return basic_instruction("OP_NOT_EQUAL", offset, file);
            break;
        case OP_GREATER:
            return basic_instruction("OP_GREATER", offset, file);
            break;
        case OP_GREATER_EQUAL:
            return basic_instruction("OP_GREATER_EQUAL", offset, file);
            break;
        case OP_LESS:
            return basic_instruction("OP_LESS", offset, file);
            break;
        case OP_LESS_EQUAL:
            return basic_instruction("OP_LESS_EQUAL", offset, file);
            break;
        case OP_PRINT:
            return basic_instruction("OP_PRINT", offset, file);
            break;
        case OP_POP:
            return basic_instruction("OP_POP", offset, file);
            break;
        case OP_DEFINE_GLOBAL:
            return basic_instruction("OP_DEFINE_GLOBAL", offset, file);
            break;
        case OP_GET_GLOBAL:
            return basic_instruction("OP_GET_GLOBAL", offset, file);
            break;
        case OP_ASSIGN:
            return basic_instruction("OP_ASSIGN", offset, file);
            break;
        case OP_PRE_INCREMENT:
            return basic_instruction("OP_PRE_INCREMENT", offset, file);
            break;
        case OP_PRE_DECREMENT:
            return basic_instruction("OP_PRE_DECREMENT", offset, file);
            break;
        default:
            DEBUG_COLOR_SET(COLOR_RED);
            printf("Unknown Instruction: %d\n", instruction);
            DEBUG_COLOR_RESET();
            return offset + 1;
    }
}

const char* resolve_token_type(token_type_t type) {
    switch(type) {
        case H_TOKEN_EOF: return "H_TOKEN_EOF";
        case H_TOKEN_COMMA: return "H_TOKEN_COMMA";
        case H_TOKEN_DOT: return "H_TOKEN_DOT";
        case H_TOKEN_PLUS: return "H_TOKEN_PLUS";
        case H_TOKEN_PLUS_PLUS: return "H_TOKEN_PLUS_PLUS";
        case H_TOKEN_MINUS: return "H_TOKEN_MINUS";
        case H_TOKEN_MINUS_MINUS: return "H_TOKEN_MINUS_MINUS";
        case H_TOKEN_STAR: return "H_TOKEN_STAR";
        case H_TOKEN_SLASH: return "H_TOKEN_SLASH";
        case H_TOKEN_EQUAL: return "H_TOKEN_EQUAL";
        case H_TOKEN_DOUBLE_EQUAL: return "H_TOKEN_DOUBLE_EQUAL";
        case H_TOKEN_BANG: return "H_TOKEN_BANG";
        case H_TOKEN_LESS: return "H_TOKEN_LESS";
        case H_TOKEN_LESS_EQUAL: return "H_TOKEN_LESS_EQUAL";
        case H_TOKEN_GREATER: return "H_TOKEN_GREATER";
        case H_TOKEN_GREATER_EQUAL: return "H_TOKEN_GREATER_EQUAL";
        case H_TOKEN_IDENTIFIER: return "H_TOKEN_IDENTIFIER";
        case H_TOKEN_TRUE: return "H_TOKEN_TRUE";
        case H_TOKEN_FALSE: return "H_TOKEN_FALSE";
        case H_TOKEN_AND: return "H_TOKEN_AND";
        case H_TOKEN_OR: return "H_TOKEN_OR";
        case H_TOKEN_IF: return "H_TOKEN_IF";
        case H_TOKEN_ELSE: return "H_TOKEN_ELSE";
        case H_TOKEN_FOR: return "H_TOKEN_FOR";
        case H_TOKEN_WHILE: return "H_TOKEN_WHILE";
        case H_TOKEN_NULL: return "H_TOKEN_NULL";
        case H_TOKEN_RETURN: return "H_TOKEN_RETURN";
        case H_TOKEN_PRINT: return "H_TOKEN_PRINT";
        case H_TOKEN_VAR: return "H_TOKEN_VAR";
        case H_TOKEN_DO: return "H_TOKEN_DO";
        case H_TOKEN_LEFT_PAR: return "H_TOKEN_LEFT_PAR";
        case H_TOKEN_RIGHT_PAR: return "H_TOKEN_RIGHT_PAR";
        case H_TOKEN_LEFT_SQUARE: return "H_TOKEN_LEFT_SQUARE";
        case H_TOKEN_RIGHT_SQUARE: return "H_TOKEN_RIGHT_SQUARE";
        case H_TOKEN_LEFT_CURLY: return "H_TOKEN_LEFT_CURLY"; 
        case H_TOKEN_RIGHT_CURLY: return "H_TOKEN_RIGHT_CURLY";
        case H_TOKEN_SEMICOLON: return "H_TOKEN_SEMICOLON";
        case H_TOKEN_COLON: return "H_TOKEN_COLON";
        case H_TOKEN_POW: return "H_TOKEN_POW";
        case H_TOKEN_ERROR: return "H_TOKEN_ERROR";
        case H_TOKEN_WARNING: return "H_TOKEN_WARNING";
        case H_TOKEN_MODULO: return "H_TOKEN_MODULO";
        case H_TOKEN_STRING_LITERAL: return "H_TOKEN_STRING_LITERAL";
        case H_TOKEN_NUMBER_LITERAL: return "H_TOKEN_NUMBER_LITERAL";
        case H_TOKEN_CLASS: return "H_TOKEN_CLASS";
        case H_TOKEN_SUPER: return "H_TOKEN_SUPER";
        case H_TOKEN_FUNCTION: return "H_TOKEN_FUNCTION";
        case H_TOKEN_BITWISE_SHIFT_LEFT: return "H_TOKEN_BITWISE_SHIFT_LEFT";
        case H_TOKEN_BITWISE_SHIFT_RIGHT: return "H_TOKEN_BITWISE_SHIFT_RIGHT";
        case H_TOKEN_BITWISE_AND: return "H_TOKEN_BITWISE_AND";
        case H_TOKEN_BITWISE_OR: return "H_TOKEN_BITWISE_OR";
        case H_TOKEN_BITWISE_XOR: return "H_TOKEN_BITWISE_XOR";
        case H_TOKEN_BITWISE_NOT: return "H_TOKEN_BITWISE_NOT";
        case H_TOKEN_BANG_EQUAL: return "H_TOKEN_BANG_EQUAL";
        case H_TOKEN_QUESTION_MARK: return "H_TOKEN_QUESTION_MARK";
        case H_TOKEN_DOUBLE_QUESTION_MARK: return "H_TOKEN_DOUBLE_QUESTION_MARK";
        case H_TOKEN_NUM: return "H_TOKEN_NUM";
        case H_TOKEN_STR: return "H_TOKEN_STR";
        case H_TOKEN_CONST: return "H_TOKEN_CONST";
        case H_TOKEN_GLOB: return "H_TOKEN_GLOB";
        case H_TOKEN_LAST: return "H_TOKEN_LAST";
        default:
            return "Add type";
    }
}

void token_print(token_t* token) {
    printf("Line: %lld - %s - '%.*s'\n", token->line, resolve_token_type(token->type), (int)token->length, token->start);
}

size_t token_write_print_string(FILE* file, token_t* token) {
    char* string = (char*)malloc((sizeof(char) * (token->length + 50)));
    sprintf(string, "Line: %lld - %s - '%.*s'\n", token->line, resolve_token_type(token->type), (int)token->length, token->start);
    size_t result = fwrite(string, 1, strlen(string), file);
    free((void*)string);
    return result;
}

#endif
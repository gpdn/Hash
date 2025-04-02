

#include "debug.h"

static int basic_instruction(const char* name, size_t offset, FILE* file);
static int constant_instruction(const char* name, bytecode_store_t* store, size_t offset, FILE* file);
static int index_instruction(const char* name, bytecode_store_t* store, size_t offset, FILE* file);
static int double_index_instruction(const char* name, bytecode_store_t* store, size_t offset, FILE* file);
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
            DEBUG_LOG("%0.2f\n", value.number);
            break;
        case H_VALUE_STRING:
            DEBUG_LOG("%s\n", value.string->string);
            break;
        case H_VALUE_CHAR:
            DEBUG_LOG("%c\n", value.character);
            break;
        case H_VALUE_ARRAY:
            print_value(&value);
            break;
        case H_VALUE_TYPE:
            print_value(&value);
            break;
        default: 
            printf("%s\n", "Unimplemented value");
            break;
    }
} 

static int constant_instruction(const char* name, bytecode_store_t* store, size_t offset, FILE* file) {
    #if DEBUG_ALL
        DEBUG_LOG("%s ", name);
        value_t value = store->constants->constants[store->code[offset + 1]];
        //value_t value = *(store->constants->current);
        bs_print_value(store, offset, value);
    #endif
    /* if(file) {
        char* string = (char*)malloc((sizeof(char) * (strlen(name) + 50)));
        //sprintf(string, "%s %0.2f\n", name, store->constants->constants[store->code[offset + 1]].number);
        sprintf(string, "%s %0.2f\n", name, store->constants->current->number);
        fwrite(string, 1, strlen(string), file);
        free((void*)string);
    } */
    return offset + 2;
    //return offset + 1;
}

static int index_instruction(const char* name, bytecode_store_t* store, size_t offset, FILE* file) {
    uint8_t value = store->code[offset + 1];
    DEBUG_LOG("%s %d\n", name, value);
    if(file) {
        char* string = (char*)malloc((sizeof(char) * (strlen(name) + 50)));
        sprintf(string, "%s %d\n", name, value);
        fwrite(string, 1, strlen(string), file);
        free((void*)string);
    }
    return offset + 2;
}

static int double_index_instruction(const char* name, bytecode_store_t* store, size_t offset, FILE* file) {
    uint8_t value = store->code[offset + 1];
    uint8_t value_2 = store->code[offset + 2];
    DEBUG_LOG("%s %d %d\n", name, value, value_2);
    if(file) {
        char* string = (char*)malloc((sizeof(char) * (strlen(name) + 50)));
        sprintf(string, "%s %d\n", name, value);
        fwrite(string, 1, strlen(string), file);
        free((void*)string);
    }
    return offset + 3;
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
        case OP_RETURN_VALUE:
            return basic_instruction("OP_RETURN_VALUE", offset, file);
            break;
        case OP_START:
            return basic_instruction("OP_START", offset, file);
            break;
        case OP_STOP:
            return basic_instruction("OP_STOP", offset, file);
            break;
        case OP_STOP_VALUE:
            return basic_instruction("OP_STOP_VALUE", offset, file);
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
        case OP_SET_PUSH_CONSTANT:
            return basic_instruction("OP_SET_PUSH_CONSTANT", offset, file);
            break;
        case OP_PUSH_CONSTANT:
            return basic_instruction("OP_PUSH_CONSTANT", offset, file);
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
        case OP_CONDITIONAL_EXPRESSION:
            return basic_instruction("OP_CONDITIONAL_EXPRESSION", offset, file);
            break;
        case OP_COPY:
            return basic_instruction("OP_COPY", offset, file);
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
        case OP_GENERATE_INTERVAL:
            return basic_instruction("OP_GENERATE_INTERVAL", offset, file);
            break;
        case OP_AND:
            return basic_instruction("OP_AND", offset, file);
            break;
        case OP_OR:
            return basic_instruction("OP_OR", offset, file);
            break;
        case OP_PRINT:
            return basic_instruction("OP_PRINT", offset, file);
            break;
        case OP_POP:
            return basic_instruction("OP_POP", offset, file);
            break;
        case OP_DEFINE_GLOBAL:
            return index_instruction("OP_DEFINE_GLOBAL", store, offset, file);
            break;
        case OP_GET_GLOBAL:
            return index_instruction("OP_GET_GLOBAL", store, offset, file);
            break;
        case OP_PRE_INCREMENT:
            return index_instruction("OP_PRE_INCREMENT", store, offset, file);
            break;
        case OP_PRE_DECREMENT:
            return index_instruction("OP_PRE_DECREMENT", store, offset, file);
            break;
        case OP_POST_INCREMENT:
            return index_instruction("OP_POST_INCREMENT", store, offset, file);
            break;
        case OP_POST_DECREMENT:
            return index_instruction("OP_POST_DECREMENT", store, offset, file);
            break;
        case OP_SET_LOCAL:
            return index_instruction("OP_SET_LOCAL", store, offset, file);
            break;
        case OP_DEFINE_LOCAL:
            return index_instruction("OP_DEFINE_LOCAL", store, offset, file);
            break;
        case OP_SET_LOCAL_ARRAY:
            return basic_instruction("OP_SET_LOCAL_ARRAY", offset, file);
            break;
        case OP_SET_LOCAL_DATA:
            return basic_instruction("OP_SET_LOCAL_DATA", offset, file);
            break;
        case OP_START_ARRAY_INITIALISATION:
            return basic_instruction("OP_START_ARRAY_INITIALISATION", offset, file);
            break;
        case OP_GET_LOCAL:
            return index_instruction("OP_GET_LOCAL", store, offset, file);
            break;
        case OP_GET_LOCAL_FUNCTION:
            return index_instruction("OP_GET_LOCAL_FUNCTION", store, offset, file);
            break;
        case OP_GET_LOCAL_NATIVE:
            return index_instruction("OP_GET_LOCAL_NATIVE", store, offset, file);
            break;
        case OP_GET_LOCAL_INDEX:
            return index_instruction("OP_GET_LOCAL_INDEX", store, offset, file);
            break;
        case OP_SET_LOCAL_INDEX:
            return index_instruction("OP_SET_LOCAL_INDEX", store, offset, file);
            break;
        case OP_GET_LOCAL_INDEX_STRING:
            return index_instruction("OP_GET_LOCAL_INDEX_STRING", store, offset, file);
            break;
        case OP_SET_LOCAL_INDEX_STRING:
            return index_instruction("OP_SET_LOCAL_INDEX_STRING", store, offset, file);
            break;
        case OP_GET_LOCAL_INDEX_COMPOUND:
            return double_index_instruction("OP_GET_LOCAL_INDEX_COMPOUND", store, offset, file);
            break;
        case OP_SET_LOCAL_INDEX_COMPOUND:
            return double_index_instruction("OP_SET_LOCAL_INDEX_COMPOUND", store, offset, file);
            break;
        case OP_GET_LOCAL_SIZE:
            return index_instruction("OP_GET_LOCAL_SIZE", store, offset, file);
            break;
        case OP_GET_LOCAL_SIZE_STRING:
            return index_instruction("OP_GET_LOCAL_SIZE", store, offset, file);
            break;
        case OP_CALL:
            return index_instruction("OP_CALL", store, offset, file);
            break;
        case OP_CALL_NATIVE:
            return index_instruction("OP_CALL_NATIVE", store, offset, file);
            break;
        case OP_JUMP_IF_FALSE:
            return index_instruction("OP_JUMP_IF_FALSE", store, offset, file);
            break;
        case OP_JUMP_IF_TRUE:
            return index_instruction("OP_JUMP_IF_TRUE", store, offset, file);
            break;
        case OP_JUMP:
            return index_instruction("OP_JUMP", store, offset, file);
            break;
        case OP_JUMP_PLACEHOLDER:
            return basic_instruction("OP_JUMP_PLACEHOLDER", offset, file);
            break;
        case OP_GOTO:
            return index_instruction("OP_GOTO", store, offset, file);
            break;
        case OP_REWIND:
            return index_instruction("OP_REWIND", store, offset, file);
            break;
        case OP_SWITCH:
            return index_instruction("OP_SWITCH", store, offset, file);
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
        case H_TOKEN_PLUS_EQUAL: return "H_TOKEN_PLUS_EQUAL";
        case H_TOKEN_MINUS_EQUAL: return "H_TOKEN_MINUS_EQUAL";
        case H_TOKEN_STAR_EQUAL: return "H_TOKEN_STAR_EQUAL";
        case H_TOKEN_SLASH_EQUAL: return "H_TOKEN_SLASH_EQUAL";
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
        case H_TOKEN_CHAR_LITERAL: return "H_TOKEN_CHAR_LITERAL";
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
        case H_TOKEN_CHAR: return "H_TOKEN_CHAR";
        case H_TOKEN_GLOB: return "H_TOKEN_GLOB";
        case H_TOKEN_LABEL: return "H_TOKEN_LABEL";
        case H_TOKEN_GOTO: return "H_TOKEN_GOTO";
        case H_TOKEN_DOUBLE_COLON: return "H_TOKEN_DOUBLE_COLON";
        case H_TOKEN_ENUM: return "H_TOKEN_ENUM";
        case H_TOKEN_DOUBLE_BANG: return "H_TOKEN_DOUBLE_BANG";
        case H_TOKEN_ARR: return "H_TOKEN_ARR";
        case H_TOKEN_REPEAT: return "H_TOKEN_REPEAT";
        case H_TOKEN_BREAK: return "H_TOKEN_BREAK";
        case H_TOKEN_SKIP: return "H_TOKEN_SKIP";
        case H_TOKEN_LOOP: return "H_TOKEN_LOOP";
        case H_TOKEN_TO: return "H_TOKEN_TO";
        case H_TOKEN_TYPE: return "H_TOKEN_TYPE";
        case H_TOKEN_DATA: return "H_TOKEN_DATA";
        case H_TOKEN_ARROW: return "H_TOKEN_ARROW";
        case H_TOKEN_STOP: return "H_TOKEN_STOP";
        case H_TOKEN_COPY: return "H_TOKEN_COPY";
        case H_TOKEN_SWITCH: return "H_TOKEN_SWITCH";
        case H_TOKEN_SELECT: return "H_TOKEN_SELECT";
        case H_TOKEN_THEN: return "H_TOKEN_THEN";
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

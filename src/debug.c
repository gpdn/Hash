#define DEBUG 1

#if DEBUG

#include "debug.h"

static int basic_instruction(const char* name, size_t offset, FILE* file);
static int constant_instruction(const char* name, bytecode_store_t* store, size_t offset, FILE* file);

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

static int constant_instruction(const char* name, bytecode_store_t* store, size_t offset, FILE* file) {
    DEBUG_LOG("%s ", name);
    DEBUG_LOG("%0.2f\n", store->constants->constants[store->code[offset + 1]]);
    if(file) {
        char* string = (char*)malloc((sizeof(char) * (strlen(name) + 50)));
        sprintf(string, "%s %0.2f\n", name, store->constants->constants[store->code[offset + 1]]);
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
        default:
            DEBUG_COLOR_SET(COLOR_RED);
            printf("Unknown Instruction: %d", instruction);
            DEBUG_COLOR_RESET();
            return offset + 1;
    }
}

void token_print(token_t* token) {
    printf("Line: %lld - %u - '%.*s'\n", token->line, token->type, (int)token->length, token->start);
}

size_t token_write_print_string(FILE* file, token_t* token) {
    char* string = (char*)malloc((sizeof(char) * (token->length + 50)));
    sprintf(string, "Line: %lld - %u - '%.*s'\n", token->line, token->type, (int)token->length, token->start);
    size_t result = fwrite(string, 1, strlen(string), file);
    free((void*)string);
    return result;
}

#endif
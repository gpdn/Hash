#define DEBUG 1

#if DEBUG

#include "debug.h"

static int basic_instruction(const char* name, size_t offset) {
    DEBUG_LOG("%s\n", name);
    return offset + 1;
}

static int constant_instruction(const char* name, bytecode_store_t* store, size_t offset) {
    DEBUG_LOG("%s ", name);
    DEBUG_LOG("%0.2f\n", store->constants->constants[store->code[offset + 1]]);
    return offset + 2;
}

void disassemble_bytecode_store(bytecode_store_t* store, const char* name) {
    DEBUG_TITLE(name);

    for(size_t offset = 0; offset < store->size;) {
        offset = disassemble_instruction(store, offset);
    }
}

size_t disassemble_instruction(bytecode_store_t* store, size_t offset) {
    printf("%04lld  ", offset);

    uint8_t instruction = store->code[offset];
    switch(instruction) {
        case OP_RETURN:
            return basic_instruction("OP_RETURN", offset);
            break;
        case OP_START:
            return basic_instruction("OP_START", offset);
            break;
        case OP_STOP:
            return basic_instruction("OP_STOP", offset);
            break;
        case OP_CONSTANT:
            return constant_instruction("OP_CONSTANT", store, offset);
            break;
        default:
            DEBUG_COLOR_SET(COLOR_RED);
            printf("Unknown Instruction: %d", instruction);
            DEBUG_COLOR_RESET();
            return offset + 1;
    }
}

#endif
#include "bytecode_store.h"

bytecode_store_t* bs_init(size_t capacity) {
    bytecode_store_t* bs = (bytecode_store_t*)malloc(sizeof(bytecode_store_t));
    bs->code = (uint8_t*)malloc(sizeof(uint8_t) * capacity);
    bs->constants = cp_init(capacity);
    bs->size = 0;
    bs->capacity = capacity;
    return bs;
}

void bs_write(bytecode_store_t* store, uint8_t byte) {
    if(store->size >= store->capacity) {
        store->capacity *= 2; 
        store->code = (uint8_t*)realloc(store->code, sizeof(uint8_t) * store->capacity);
    }
    store->code[store->size++] = byte; 
}

void bs_write_constant(bytecode_store_t* store, number_t constant) {
    size_t index = cp_write(store->constants, constant);
    bs_write(store, OP_CONSTANT);
    bs_write(store, index);
}

void bs_free(bytecode_store_t* store) {
    free(store->code);
    cp_free(store->constants);
    free(store);
}


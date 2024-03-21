
#define DEBUG 1

#include "headers.h"
#include "debug.h"
#include "bytecode_store.h"

int main() {

    DEBUG_TITLE("Start");

    bytecode_store_t* store = bs_init(50);
    bs_write(store, OP_RETURN);
    bs_write_constant(store, 10);

    disassemble_bytecode_store(store, "Bytecode Store");

    bs_free(store);

    return 0;
}
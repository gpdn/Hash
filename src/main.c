
#include "headers.h"
#include "debug.h"
#include "h_file.h"
#include "bytecode_store.h"
#include "virtual_machine.h"
#include "h_pipeline.h"

#define PATH "./test.hash"

int main() {

    interpreter_result_t result = pipeline_start(PATH);

    return 0;

    bytecode_store_t* store = bs_init(50);
    
    bs_write(store, OP_START);
    bs_write_constant(store, 10);
    bs_write_constant(store, 5);
    bs_write(store, OP_ADD);
    bs_write_constant(store, 10);
    bs_write(store, OP_SUB);
    bs_write(store, OP_NEGATE);
    bs_write(store, OP_RETURN);
    bs_write(store, OP_STOP);
    
    disassemble_bytecode_store(store, "Bytecode Store");

    virtual_machine_t* vm = vm_init(store);

    vm_run(vm);

    bs_free(store);
    vm_free(vm);

    return 0;
}
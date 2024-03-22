
#include "headers.h"
#include "debug.h"
#include "bytecode_store.h"
#include "virtual_machine.h"

int main() {

    bytecode_store_t* store = bs_init(50);
    
    bs_write(store, OP_START);
    bs_write(store, OP_RETURN);
    bs_write_constant(store, 10);
    bs_write(store, OP_STOP);
    
    disassemble_bytecode_store(store, "Bytecode Store");

    virtual_machine_t* vm = vm_init(store);

    interpreter_result_t result = vm_run(vm);

    bs_free(store);
    vm_free(vm);

    return 0;
}
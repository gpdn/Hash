#include "virtual_machine.h"

virtual_machine_t *vm_init(bytecode_store_t *store)
{
    virtual_machine_t* vm = (virtual_machine_t*)malloc(sizeof(virtual_machine_t));
    vm->store = store;
    vm->instruction_pointer = store->code;
    return vm;
}

interpreter_result_t vm_run(virtual_machine_t* vm) {
    #define ADVANCE_INSTRUCTION_POINTER() (*vm->instruction_pointer++) 
    #define READ_CONSTANT() vm->store->constants->constants[ADVANCE_INSTRUCTION_POINTER()] 

    #if DEBUG_TRACE_VM 
        DEBUG_COLOR_SET(COLOR_BLUE);
            disassemble_bytecode_store(vm->store, "VM STORE");
        DEBUG_COLOR_RESET(COLOR_BLUE);
    #endif

    uint8_t instruction;
    while((instruction = ADVANCE_INSTRUCTION_POINTER()) != OP_STOP) {
        
        #if DEBUG_TRACE_VM
            disassemble_instruction(vm->store, (size_t)(vm->instruction_pointer - vm->store->code - 1));
        #endif

        switch(instruction) {
            case OP_STOP:
                return VM_SUCCESS;
            case OP_START:
                DEBUG_ERROR("Already Started: ");
                disassemble_instruction(vm->store, (size_t)(vm->instruction_pointer - vm->store->code - 1));
                break;
            case OP_CONSTANT:
                DEBUG_LOG("Value: %0.2f", READ_CONSTANT());
                break;
            default:
                DEBUG_ERROR("Unimplemented instruction: "); 
                disassemble_instruction(vm->store, (size_t)(vm->instruction_pointer - vm->store->code - 1));
                return VM_ERROR;

        }

    }
    
    return VM_ERROR;

}

void vm_free(virtual_machine_t* vm) {
    free(vm);
}
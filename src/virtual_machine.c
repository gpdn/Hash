#include "virtual_machine.h"

static void vm_stack_push(virtual_machine_t* vm, value_t value);
static value_t vm_stack_pop(virtual_machine_t* vm);

static void vm_stack_push(virtual_machine_t* vm, value_t value) {
    *vm->stack_top = value;
    ++vm->stack_top;
}

static value_t vm_stack_pop(virtual_machine_t* vm) {
    return *(--vm->stack_top);
}

virtual_machine_t* vm_init(bytecode_store_t *store)
{
    virtual_machine_t* vm = (virtual_machine_t*)malloc(sizeof(virtual_machine_t));
    vm->stack = (value_t*)malloc(sizeof(value_t) * store->constants->capacity);
    vm->stack_top = vm->stack;
    vm->store = store;
    vm->instruction_pointer = store->code;
    return vm;
}

interpreter_result_t vm_run(virtual_machine_t* vm) {
    #define ADVANCE_INSTRUCTION_POINTER() (*vm->instruction_pointer++) 
    #define READ_CONSTANT() vm->store->constants->constants[ADVANCE_INSTRUCTION_POINTER()] 
    #define BINARY_OP(op) vm_stack_push(vm, vm_stack_pop(vm) op vm_stack_pop(vm))
    #define BINARY_OP_ASSOC(name, op) value_t name = vm_stack_pop(vm); vm_stack_push(vm, vm_stack_pop(vm) op name);
    #define BITWISE_OP_ASSOC(name, op) value_t name = vm_stack_pop(vm); vm_stack_push(vm, (unsigned int)vm_stack_pop(vm) op (unsigned int)name);

    #if DEBUG_TRACE_VM_BYTECODE
        DEBUG_COLOR_SET(COLOR_CYAN);
        DEBUG_TITLE("VM");
        DEBUG_COLOR_RESET();
    #endif

    uint8_t instruction;
    while((instruction = ADVANCE_INSTRUCTION_POINTER()) != OP_STOP) {
        
        #if DEBUG_TRACE_VM_BYTECODE
            disassemble_instruction(vm->store, (size_t)(vm->instruction_pointer - vm->store->code - 1), NULL);
        #endif

        switch(instruction) {
            case OP_STOP:
                return VM_SUCCESS;
            case OP_START:
                DEBUG_ERROR("Already Started");
                disassemble_instruction(vm->store, (size_t)(vm->instruction_pointer - vm->store->code - 1), NULL);
                break;
            case OP_CONSTANT:
                value_t value = READ_CONSTANT();
                vm_stack_push(vm, value);
                break;
            case OP_NEGATE:
                vm_stack_push(vm, -vm_stack_pop(vm));
                break;
            case OP_ADD:
                BINARY_OP(+);
                break;
            case OP_SUB:
                BINARY_OP(-);
                break;
            case OP_MUL:
                BINARY_OP(*);
                break;
            case OP_DIV:
                BINARY_OP_ASSOC(div_val, /);
                break;
            case OP_SHIFT_LEFT:
                BITWISE_OP_ASSOC(shift_left_val, <<);
                break;
            case OP_SHIFT_RIGHT:
                BITWISE_OP_ASSOC(shift_right_val, >>);
                break;
            case OP_BITWISE_AND:
                BITWISE_OP_ASSOC(bitwise_and_val, &);
                break;
            case OP_BITWISE_OR:
                BITWISE_OP_ASSOC(bitwise_or_val, |);
                break;
            case OP_BITWISE_XOR:
                BITWISE_OP_ASSOC(bitwise_xor_val, ^);
                break;
            case OP_BITWISE_NOT:
                vm_stack_push(vm, ~(unsigned int)vm_stack_pop(vm));
                break;
            case OP_RETURN:
                value_t result = vm_stack_pop(vm);
                DEBUG_LOG("OP_RETURN %0.2f\n", result);
                return VM_SUCCESS;
                break;
            default:
                DEBUG_ERROR("Unimplemented instruction: "); 
                disassemble_instruction(vm->store, (size_t)(vm->instruction_pointer - vm->store->code - 1), NULL);
                return VM_ERROR;

        }

        #if DEBUG_TRACE_VM_STACK
            for(value_t* temp = vm->stack; temp < vm->stack_top; ++temp) {
                DEBUG_COLOR_SET(COLOR_CYAN);
                DEBUG_LOG("[%0.2f]\n", *temp);
                DEBUG_COLOR_RESET();
            }
        #endif

    }
    
    return VM_ERROR;

}

void vm_free(virtual_machine_t* vm) {
    free(vm->stack);
    free(vm);
}
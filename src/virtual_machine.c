#include "virtual_machine.h"

static void vm_stack_push(virtual_machine_t* vm, value_t value);
static inline value_t vm_stack_pop(virtual_machine_t* vm);
static inline value_t vm_stack_peek(virtual_machine_t* vm);
static const char* resolve_type(value_t* value);
static inline void resolve_value(value_t* value);

static const char* resolve_type(value_t* value) {
    switch(value->type) {
        case H_VALUE_NUMBER:
            return "Num";
        case H_VALUE_STRING:
            return "Str";
        default:
            return "Unk";
    }
}

static void vm_stack_push(virtual_machine_t* vm, value_t value) {
    *vm->stack_top = value;
    ++vm->stack_top;
}

static inline value_t vm_stack_pop(virtual_machine_t* vm) {
    return *(--vm->stack_top);
}

static inline value_t vm_stack_peek(virtual_machine_t* vm) {
    return *(vm->stack_top - 1);
}

virtual_machine_t* vm_init(bytecode_store_t *store)
{
    virtual_machine_t* vm = (virtual_machine_t*)malloc(sizeof(virtual_machine_t));
    vm->stack = (value_t*)malloc(sizeof(value_t) * store->constants->capacity);
    vm->stack_top = vm->stack;
    vm->store = store;
    vm->instruction_pointer = store->code;
    vm->globals_table = h_hash_table_init(200, 0.75);
    return vm;
}

interpreter_result_t vm_run(virtual_machine_t* vm) {
    #define ADVANCE_INSTRUCTION_POINTER() (*vm->instruction_pointer++) 
    #define READ_CONSTANT() vm->store->constants->constants[ADVANCE_INSTRUCTION_POINTER()] 
    #define BINARY_OP(type, op) vm_stack_push(vm, type(vm_stack_pop(vm).number op vm_stack_pop(vm).number))
    #define BINARY_OP_ASSOC(name, type, op) value_t name = vm_stack_pop(vm); vm_stack_push(vm, type(vm_stack_pop(vm).number op name.number));
    #define BITWISE_OP_ASSOC(name, op) value_t name = vm_stack_pop(vm); vm_stack_push(vm, NUM_VALUE(((unsigned int)(vm_stack_pop(vm).number)) op ((unsigned int)(name.number))));

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
                vm_stack_push(vm, NUM_VALUE(-vm_stack_pop(vm).number));
                break;
            case OP_ADD:
                value_t add_val = vm_stack_pop(vm);
                switch(add_val.type) {
                    case H_VALUE_NUMBER:
                        vm_stack_push(vm, NUM_VALUE(vm_stack_pop(vm).number + add_val.number));
                        break;
                    case H_VALUE_STRING:
                        vm_stack_push(vm, STR_VALUE(h_string_concatenate(vm_stack_pop(vm).string, add_val.string)));
                        break;
                    default:
                        return VM_ERROR;
                }
                //BINARY_OP_ASSOC(add_val, NUM_VALUE, +);
                break;
            case OP_SUB:
                BINARY_OP(NUM_VALUE, -);
                break;
            case OP_MUL:
                BINARY_OP(NUM_VALUE, *);
                break;
            case OP_DIV:
                BINARY_OP_ASSOC(div_val, NUM_VALUE, /);
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
                vm_stack_push(vm, NUM_VALUE(~(unsigned int)vm_stack_pop(vm).number));
                break;
            case OP_EQUALITY:
                BINARY_OP(NUM_VALUE, ==);
                break;
            case OP_NOT_EQUAL:
                BINARY_OP(NUM_VALUE, !=);
                break;
            case OP_GREATER:
                BINARY_OP_ASSOC(greater_val, NUM_VALUE, >);
                break;
            case OP_GREATER_EQUAL:
                BINARY_OP_ASSOC(greater_equal_val, NUM_VALUE, >=);
                break;
            case OP_LESS:
                BINARY_OP_ASSOC(less_val, NUM_VALUE, <);
                break;
            case OP_LESS_EQUAL:
                BINARY_OP_ASSOC(less_equal_val, NUM_VALUE, <=);
                break;
            case OP_RETURN:
                /* value_t result = vm_stack_pop(vm);
                resolve_value(&result); */
                return VM_SUCCESS;
            case OP_PRINT:
                value_t print_val = vm_stack_pop(vm);
                print_value(&print_val);
                break;
            case OP_POP:
                vm_stack_pop(vm);
                break;
            case OP_DEFINE_GLOBAL:
                value_t name = vm_stack_pop(vm);
                //value_t ht_value = vm_stack_pop(vm);
                h_ht_set(vm->globals_table, name.string, vm_stack_pop(vm));
                //h_ht_print(vm->globals_table);
                break;
            case OP_GET_GLOBAL:
                value_t ht_value = h_ht_get(vm->globals_table, vm_stack_pop(vm).string);
                print_value(&ht_value);
                vm_stack_push(vm, ht_value);
                //h_ht_print(vm->globals_table);
                break;
            case OP_ASSIGN:
                value_t ht_assign = vm_stack_pop(vm);
                h_ht_set(vm->globals_table, ht_assign.string, vm_stack_peek(vm));
                break;
            case OP_PRE_INCREMENT:
                value_t ht_pre_increase = vm_stack_pop(vm);
                vm_stack_push(vm, h_ht_increase(vm->globals_table, ht_pre_increase.string));
                break;
            case OP_PRE_DECREMENT:
                value_t ht_pre_decrease = vm_stack_pop(vm);
                vm_stack_push(vm, h_ht_decrease(vm->globals_table, ht_pre_decrease.string));
                break;
            default:
                DEBUG_ERROR("Unimplemented instruction: "); 
                disassemble_instruction(vm->store, (size_t)(vm->instruction_pointer - vm->store->code - 1), NULL);
                return VM_ERROR;

        }

        #if DEBUG_TRACE_VM_STACK
            for(value_t* temp = vm->stack; temp < vm->stack_top; ++temp) {
                DEBUG_COLOR_SET(COLOR_CYAN);
                resolve_value(temp);
                DEBUG_COLOR_RESET();
            }
        #endif

    }
    
    return VM_ERROR;

}

static inline void resolve_value(value_t* value) {
    switch(value->type) {
        case H_VALUE_NUMBER:
            DEBUG_LOG("[%s, %0.2f]\n", "Num", value->number);
            break;
        case H_VALUE_STRING:
            DEBUG_LOG("[%s, %s]\n", "Str", value->string->string);
            break;
    }
}

void vm_free(virtual_machine_t* vm) {
    free(vm->stack);
    free(vm);
}
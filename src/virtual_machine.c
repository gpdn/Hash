#include "virtual_machine.h"

static void vm_stack_push(virtual_machine_t* vm, value_t value);
static inline value_t vm_stack_pop(virtual_machine_t* vm);
static inline value_t vm_stack_peek(virtual_machine_t* vm);
static inline void resolve_value(value_t* value);

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

virtual_machine_t* vm_init(bytecode_store_t *store, h_hash_table_t* globals_table, h_locals_stack_t* locals_stack)
{
    virtual_machine_t* vm = (virtual_machine_t*)malloc(sizeof(virtual_machine_t));
    vm->stack = (value_t*)malloc(sizeof(value_t) * store->constants->capacity);
    vm->stack_top = vm->stack;
    vm->store = store;
    vm->instruction_pointer = store->code;
    vm->globals_table = globals_table;
    vm->locals_stack = locals_stack;
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
                //value_t name = vm_stack_pop(vm);
                //value_t ht_value = vm_stack_pop(vm);
                h_ht_array_set(vm->globals_table, ADVANCE_INSTRUCTION_POINTER(), vm_stack_pop(vm));
                //h_ht_set(vm->globals_table, name.string, vm_stack_pop(vm));
                //h_ht_print(vm->globals_table);
                break;
            case OP_SET_LOCAL:
                //value_t name = vm_stack_pop(vm);
                //value_t ht_value = vm_stack_pop(vm);
                //h_local_set(vm->globals_table, ADVANCE_INSTRUCTION_POINTER(), vm_stack_pop(vm));
                //h_ht_set(vm->globals_table, name.string, vm_stack_pop(vm));
                //h_ht_print(vm->globals_table);
                h_locals_array_set(vm->locals_stack, ADVANCE_INSTRUCTION_POINTER(), vm_stack_pop(vm));
                break;
            case OP_GET_GLOBAL:
                //value_t ht_value = h_ht_get(vm->globals_table, vm_stack_pop(vm).string);
                vm_stack_push(vm, h_ht_array_get(vm->globals_table, ADVANCE_INSTRUCTION_POINTER()));
                break;
            case OP_GET_LOCAL:
                value_t local_value = h_locals_array_get(vm->locals_stack, ADVANCE_INSTRUCTION_POINTER());
                //value_t ht_value = h_ht_get(vm->globals_table, vm_stack_pop(vm).string);
                vm_stack_push(vm, local_value);
                //h_ht_print(vm->globals_table);
                break;
            case OP_ASSIGN:
                h_locals_array_set(vm->locals_stack, ADVANCE_INSTRUCTION_POINTER(), vm_stack_pop(vm));
                break;
            case OP_PRE_INCREMENT:
                vm_stack_push(vm, h_locals_array_increase_get(vm->locals_stack, ADVANCE_INSTRUCTION_POINTER()));
                break;
            case OP_PRE_DECREMENT:
                vm_stack_push(vm, h_locals_array_increase_get(vm->locals_stack, ADVANCE_INSTRUCTION_POINTER()));
                break;
            case OP_POST_INCREMENT:
                vm_stack_push(vm, h_locals_array_post_increase_get(vm->locals_stack, ADVANCE_INSTRUCTION_POINTER()));
                break;
            case OP_POST_DECREMENT:
                vm_stack_push(vm, h_locals_array_post_increase_get(vm->locals_stack, ADVANCE_INSTRUCTION_POINTER()));
                break;
            case OP_JUMP_IF_FALSE:
                value_t ht_jump_if_false = vm_stack_pop(vm);
                size_t ht_jump_if_false_jump = ADVANCE_INSTRUCTION_POINTER();
                if(ht_jump_if_false.number == 0) {vm->instruction_pointer = vm->store->code + ht_jump_if_false_jump;}
                break;
            case OP_JUMP:
                size_t ht_jump = ADVANCE_INSTRUCTION_POINTER();
                vm->instruction_pointer = vm->store->code + ht_jump;
                break;
            case OP_GOTO:
                //value_t ht_jump_if_false = vm_stack_pop(vm);
                size_t ht_goto = ADVANCE_INSTRUCTION_POINTER();
                vm->instruction_pointer = vm->store->code + ht_goto;
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
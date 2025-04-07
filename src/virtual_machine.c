#include "virtual_machine.h"

#define H_MAX_CALLS_STACK_SIZE 256
#define H_VM_STACK_MIN_CAPACITY 500

static void vm_stack_push(virtual_machine_t* vm, value_t value);
static inline value_t vm_stack_pop(virtual_machine_t* vm);
static inline value_t vm_stack_peek(virtual_machine_t* vm);
static inline value_t vm_stack_get(virtual_machine_t* vm, size_t index);
static inline value_t vm_stack_get_absolute(virtual_machine_t* vm, size_t index);
static inline void vm_stack_set(virtual_machine_t* vm, size_t index, value_t value);
static inline value_t vm_stack_pre_increase(virtual_machine_t* vm, size_t index);
static inline value_t vm_stack_pre_decrease(virtual_machine_t* vm, size_t index);
static inline value_t vm_stack_post_increase(virtual_machine_t* vm, size_t index);
static inline value_t vm_stack_post_decrease(virtual_machine_t* vm, size_t index);
static inline value_t vm_stack_get_index(virtual_machine_t* vm, size_t index, size_t element_index);
static inline void vm_stack_set_index(virtual_machine_t* vm, size_t index, size_t element_index, value_t value);
static inline value_t vm_stack_get_index_string(virtual_machine_t* vm, size_t index, size_t element_index);
static inline void vm_stack_set_index_string(virtual_machine_t* vm, size_t index, size_t element_index, value_t value);
static inline void resolve_value(value_t* value);
static inline void call_frame_print(call_frame_t* frame);

static inline void call_frame_print(call_frame_t* frame) {
    DEBUG_LOG("Function: \n");
    h_function_print(frame->function);
    DEBUG_LOG("Return Instruction: %d\n", *frame->return_instruction);
    print_value(frame->frame_stack);
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

static inline value_t vm_stack_get(virtual_machine_t* vm, size_t index) {
    return *(vm->stack_base + index);
}

static inline value_t vm_stack_get_absolute(virtual_machine_t* vm, size_t index) {
    return *(vm->stack + index);
}

static inline void vm_stack_set(virtual_machine_t* vm, size_t index, value_t value) {
    *(vm->stack_base + index) = value;
}

static inline value_t vm_stack_pre_increase(virtual_machine_t* vm, size_t index) {
    ++(vm->stack_base + index)->number;
    return *(vm->stack_base + index);
}

static inline value_t vm_stack_pre_decrease(virtual_machine_t* vm, size_t index) {
    --(vm->stack_base + index)->number;
    return *(vm->stack_base + index);
}

static inline value_t vm_stack_post_increase(virtual_machine_t* vm, size_t index) {
    value_t value = *(vm->stack_base + index); 
    ++(vm->stack_base + index)->number;
    return value;
}

static inline value_t vm_stack_post_decrease(virtual_machine_t* vm, size_t index) {
    value_t value = *(vm->stack_base + index); 
    --(vm->stack_base + index)->number;
    return value;
}

static inline value_t vm_stack_get_index(virtual_machine_t* vm, size_t index, size_t element_index) {
    return (*(vm->stack_base + index)).array->data[element_index];
}

static inline void vm_stack_set_index(virtual_machine_t* vm, size_t index, size_t element_index, value_t value) {
    (*(vm->stack_base + index)).array->data[element_index] = value;
}

static inline value_t vm_stack_get_index_string(virtual_machine_t* vm, size_t index, size_t element_index) {
    return CHAR_VALUE((*(vm->stack_base + index)).string->string[element_index]);
}

static inline void vm_stack_set_index_string(virtual_machine_t* vm, size_t index, size_t element_index, value_t value) {
    (*(vm->stack_base + index)).string->string[element_index] = value.character;
}

virtual_machine_t* vm_init(bytecode_store_t *store, h_hash_table_t* globals_table, h_locals_stack_t* locals_stack, h_switch_tables_list_t* switch_tables_list)
{
    virtual_machine_t* vm = (virtual_machine_t*)malloc(sizeof(virtual_machine_t));
    vm->stack = (value_t*)malloc(sizeof(value_t) * store->constants->capacity + sizeof(value_t) * locals_stack->size + sizeof(value_t) * H_VM_STACK_MIN_CAPACITY);
    vm->stack_top = vm->stack;
    vm->store = store;
    vm->initial_store = store;
    vm->instruction_pointer = store->code;
    vm->globals_table = globals_table;
    vm->locals_stack = locals_stack;
    vm->initial_locals_stack = locals_stack;
    vm->switch_tables_list = switch_tables_list;
    vm->array_initialisation_ptr = vm->stack;
    vm->stack_base = vm->stack;
    vm->calls_stack_size = 0;
    return vm;
}

int vm_run(virtual_machine_t* vm) {
    
    for(h_local_t* it = vm->locals_stack->locals_array; it != vm->locals_stack->locals_stack_top; ++it) {
        if(it->value.type != H_VALUE_NATIVE) break;
        vm_stack_push(vm, it->value);
    }

    #define ADVANCE_INSTRUCTION_POINTER() (*vm->instruction_pointer++) 
    #define READ_CONSTANT() vm->store->constants->constants[ADVANCE_INSTRUCTION_POINTER()] 
    #define READ_PUSH_CONSTANT() (*vm->store->constants->current++)
    #define BINARY_OP(type, op) vm_stack_push(vm, type(vm_stack_pop(vm).number op vm_stack_pop(vm).number))
    #define BINARY_OP_ASSOC(name, type, op) value_t name = vm_stack_pop(vm); vm_stack_push(vm, type(vm_stack_pop(vm).number op name.number));
    #define BITWISE_OP_ASSOC(name, op) value_t name = vm_stack_pop(vm); vm_stack_push(vm, NUM_VALUE(((unsigned int)(vm_stack_pop(vm).number)) op ((unsigned int)(name.number))));

    #if DEBUG_TRACE_VM_BYTECODE
        DEBUG_COLOR_SET(COLOR_CYAN);
        DEBUG_TITLE("VM");
        DEBUG_COLOR_RESET();
    #endif

    uint8_t instruction;
    while(instruction = ADVANCE_INSTRUCTION_POINTER()) {

        #if DEBUG_TRACE_VM_BYTECODE
            disassemble_instruction(vm->store, (size_t)(vm->instruction_pointer - vm->store->code - 1), NULL);
        #endif

        switch(instruction) {
            case OP_STOP: 
                return VM_SUCCESS;
            case OP_STOP_VALUE:
                return (int)(vm_stack_pop(vm).number);
            case OP_START:
                DEBUG_ERROR("Already Started");
                disassemble_instruction(vm->store, (size_t)(vm->instruction_pointer - vm->store->code - 1), NULL);
                break;
            case OP_CONSTANT:
                value_t value = READ_CONSTANT();
                vm_stack_push(vm, value);
                break;
            case OP_PUSH_CONSTANT:
                vm_stack_push(vm, READ_PUSH_CONSTANT());
                break;
            case OP_SET_PUSH_CONSTANT:
                vm_stack_push(vm, READ_PUSH_CONSTANT());
                break;
            case OP_NEGATE:
                vm_stack_push(vm, NUM_VALUE(-vm_stack_pop(vm).number));
                break;
            case OP_ADD:
                value_t add_val = vm_stack_pop(vm);
                value_t add_val_1 = vm_stack_pop(vm);
                switch(add_val.type) {
                    case H_VALUE_NUMBER:
                        vm_stack_push(vm, NUM_VALUE(add_val_1.number + add_val.number));
                        break;
                    case H_VALUE_STRING:
                        if(add_val_1.type == H_VALUE_STRING) {
                            vm_stack_push(vm, STR_VALUE(h_string_concatenate(add_val_1.string, add_val.string)));
                            break;
                        }
                        vm_stack_push(vm, STR_VALUE(h_string_concatenate_char(add_val.string, add_val_1.character)));
                        break;
                    case H_VALUE_CHAR:
                        if(add_val_1.type == H_VALUE_STRING) {
                            vm_stack_push(vm, STR_VALUE(h_string_concatenate_pre_char(add_val_1.string, add_val.character)));
                            break;
                        }
                        vm_stack_push(vm, STR_VALUE(h_string_from_chars(add_val_1.character, add_val.character)));
                        break;
                    default:
                        return VM_ERROR;
                }
                break;
            case OP_SUB:
                BINARY_OP_ASSOC(sub_val, NUM_VALUE, -);
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
            case OP_AND:
                BINARY_OP_ASSOC(and_val, NUM_VALUE, &&);
                break;
            case OP_OR:
                BINARY_OP_ASSOC(or_val, NUM_VALUE, ||);
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
            case OP_CONDITIONAL_EXPRESSION:
                value_t conditional_else_value = vm_stack_pop(vm);
                value_t conditional_if_value = vm_stack_pop(vm);
                vm_stack_pop(vm).number != 0 ? vm_stack_push(vm, conditional_if_value) : vm_stack_push(vm, conditional_else_value);
                break;
            case OP_GENERATE_INTERVAL:
                value_t value_one = vm_stack_pop(vm);
                value_t value_two = vm_stack_pop(vm);
                h_array_t* array = h_array_init(H_VALUE_NUMBER, value_two.number); 
                for(int i = value_one.number; i < value_two.number; ++i) h_array_push(array, NUM_VALUE(i));
                break;
            case OP_COPY:
                vm_stack_push(vm, copy_value(vm_stack_pop(vm)));
                break;
            case OP_CALL:
                size_t arguments_count = ADVANCE_INSTRUCTION_POINTER();
                h_function_t* function = (vm->stack_top - arguments_count - 1)->function;
                value_t* frame_stack_start = vm->stack_top - arguments_count;
                call_frame_t frame = {function, frame_stack_start, vm->instruction_pointer, vm->locals_stack};                
                vm->calls_stack[vm->calls_stack_size++] = frame;
                vm->instruction_pointer = frame.function->store->code;
                vm->stack_base = frame_stack_start;
                vm->store = frame.function->store;
                vm->locals_stack = frame.function->locals_stack;
                //call_frame_print(vm->calls_stack + vm->calls_stack_size);
                break;
            case OP_CALL_NATIVE:
                size_t native_arguments_count = ADVANCE_INSTRUCTION_POINTER();
                h_native_t* native_function = (vm->stack_top - native_arguments_count - 1)->native_fn;
                value_t* values = (value_t*)malloc(sizeof(value_t) * native_arguments_count);
                for(size_t i = native_arguments_count; i != 0; --i) {
                    values[i - 1] = vm_stack_pop(vm);
                }
                value_t native_return_value = native_function->fn(values, native_arguments_count);
                vm_stack_pop(vm);
                vm_stack_push(vm, native_return_value);
                break;
            case OP_RETURN:
                vm->instruction_pointer = vm->calls_stack[--vm->calls_stack_size].return_instruction;
                vm->stack_top = vm->calls_stack[vm->calls_stack_size].frame_stack;
                vm->stack_base = vm->calls_stack[vm->calls_stack_size].frame_stack;
                vm->locals_stack = vm->calls_stack[vm->calls_stack_size].locals_stack;
                if(vm->calls_stack_size > 0) {
                    vm->store = vm->calls_stack[vm->calls_stack_size].function->store;
                    vm->locals_stack = vm->calls_stack[vm->calls_stack_size].locals_stack;
                    break;
                }
                vm->store = vm->initial_store;
                vm->locals_stack = vm->initial_locals_stack;
                vm->stack_base = vm->stack;
                break;
            case OP_RETURN_VALUE:
                value_t return_value = copy_value(vm_stack_pop(vm));
                vm->instruction_pointer = vm->calls_stack[--vm->calls_stack_size].return_instruction;
                vm->stack_top = vm->calls_stack[vm->calls_stack_size].frame_stack - 1;
                vm->stack_base = vm->calls_stack[vm->calls_stack_size].frame_stack;
                if(vm->calls_stack_size > 0) {
                    vm->store = vm->calls_stack[vm->calls_stack_size].function->store;
                    vm->locals_stack = vm->calls_stack[vm->calls_stack_size].locals_stack;
                    vm_stack_push(vm, return_value);
                    break;
                }
                vm->store = vm->initial_store;
                vm->locals_stack = vm->initial_locals_stack;
                vm->stack_base = vm->stack;
                vm_stack_push(vm, return_value);
                break;
            case OP_PRINT:
                value_t print_val = vm_stack_pop(vm);
                print_value(&print_val);
                break;
            case OP_POP:
                vm_stack_pop(vm);
                break;
            case OP_DEFINE_GLOBAL:
                h_ht_array_set(vm->globals_table, ADVANCE_INSTRUCTION_POINTER(), vm_stack_pop(vm));
                break;
            case OP_DEFINE_LOCAL:
                vm_stack_push(vm, h_locals_array_get(vm->locals_stack, ADVANCE_INSTRUCTION_POINTER()));
                break;
            case OP_SET_LOCAL:
                vm_stack_set(vm, ADVANCE_INSTRUCTION_POINTER(), vm_stack_peek(vm));
                break;
            case OP_START_ARRAY_INITIALISATION:
                vm->array_initialisation_ptr = vm->stack_top;
                break;
            case OP_SET_LOCAL_ARRAY:
                value_t local_array = *(vm->array_initialisation_ptr - 1);
                for(size_t i = vm->stack_top - vm->array_initialisation_ptr; vm->stack_top != vm->stack_top - i; --i) h_array_push(local_array.array, *(vm->stack_top - i));
                vm->stack_top = vm->array_initialisation_ptr;
                break;
            case OP_SET_LOCAL_DATA:
                value_t local_data = *(vm->array_initialisation_ptr - 1);
                local_data.data_type->size = 0;
                for(size_t i = vm->stack_top - vm->array_initialisation_ptr; vm->stack_top != vm->stack_top - i; --i) h_data_push(local_data.data_type, *(vm->stack_top - i));
                vm->stack_top = vm->array_initialisation_ptr;
                //vm_stack_push(vm, NULL_VALUE());
                break;
            case OP_SET_LOCAL_INDEX:
                size_t index = vm_stack_pop(vm).number;
                vm_stack_set_index(vm, ADVANCE_INSTRUCTION_POINTER(), index, vm_stack_peek(vm));
                break;
            case OP_SET_LOCAL_INDEX_STRING:
                size_t string_index = vm_stack_pop(vm).number;
                vm_stack_set_index_string(vm, ADVANCE_INSTRUCTION_POINTER(), string_index, vm_stack_peek(vm));
                break;
            case OP_SET_LOCAL_INDEX_COMPOUND:
                size_t iterable_set_index = ADVANCE_INSTRUCTION_POINTER();
                size_t index_set_loop_count = ADVANCE_INSTRUCTION_POINTER();
                value_t index_set_value = vm_stack_get(vm, iterable_set_index);
                value_t* index_set_value_ref = &index_set_value;
                for(size_t i = 0; i < index_set_loop_count; ++i) {
                    index_set_value_ref = index_set_value_ref->array->data + (size_t)vm_stack_pop(vm).number;
                }
                *index_set_value_ref = vm_stack_peek(vm);
                vm_stack_set(vm, iterable_set_index, index_set_value);
                break;
            case OP_GET_GLOBAL:
                vm_stack_push(vm, h_ht_array_get(vm->globals_table, ADVANCE_INSTRUCTION_POINTER()));
                break;
            case OP_GET_LOCAL:
                vm_stack_push(vm, vm_stack_get(vm, ADVANCE_INSTRUCTION_POINTER()));
                break;
            case OP_GET_LOCAL_FUNCTION:
                vm_stack_push(vm, vm_stack_get_absolute(vm, ADVANCE_INSTRUCTION_POINTER()));
                break;
            case OP_GET_LOCAL_NATIVE:
                vm_stack_push(vm, vm_stack_get_absolute(vm, ADVANCE_INSTRUCTION_POINTER()));
                break;
            case OP_GET_LOCAL_INDEX:
                vm_stack_push(vm, vm_stack_get_index(vm, ADVANCE_INSTRUCTION_POINTER(), vm_stack_pop(vm).number));
                break;
            case OP_GET_LOCAL_INDEX_STRING:
                vm_stack_push(vm, vm_stack_get_index_string(vm, ADVANCE_INSTRUCTION_POINTER(), vm_stack_pop(vm).number));
                break;
            case OP_GET_LOCAL_INDEX_COMPOUND:
                size_t iterable_index = ADVANCE_INSTRUCTION_POINTER();
                size_t index_loop_count = ADVANCE_INSTRUCTION_POINTER();
                value_t index_value = vm_stack_get_index(vm, iterable_index, vm_stack_pop(vm).number);
                for(size_t i = 0; i < index_loop_count - 1; ++i) {
                    index_value = index_value.array->data[(size_t)vm_stack_pop(vm).number];
                }
                vm_stack_push(vm, index_value);
                break;
            case OP_GET_LOCAL_SIZE:
                value_t array_value = vm_stack_get(vm, ADVANCE_INSTRUCTION_POINTER());
                vm_stack_push(vm, NUM_VALUE(array_value.array->size - 1));
                break;
            case OP_GET_LOCAL_SIZE_STRING:
                value_t string_value = vm_stack_get(vm, ADVANCE_INSTRUCTION_POINTER());
                vm_stack_push(vm, NUM_VALUE(string_value.string->length - 1));
                break;
            case OP_PRE_INCREMENT:
                vm_stack_push(vm, vm_stack_pre_increase(vm, ADVANCE_INSTRUCTION_POINTER()));
                break;
            case OP_PRE_DECREMENT:
                vm_stack_push(vm, vm_stack_pre_decrease(vm, ADVANCE_INSTRUCTION_POINTER()));
                break;
            case OP_POST_INCREMENT:
                vm_stack_push(vm, vm_stack_post_increase(vm, ADVANCE_INSTRUCTION_POINTER()));
                break;
            case OP_POST_DECREMENT:
                vm_stack_push(vm, vm_stack_post_decrease(vm, ADVANCE_INSTRUCTION_POINTER()));
                break;
            case OP_JUMP_IF_FALSE:
                value_t ht_jump_if_false = vm_stack_pop(vm);
                size_t ht_jump_if_false_jump = ADVANCE_INSTRUCTION_POINTER();
                if(ht_jump_if_false.number == 0) {vm->instruction_pointer = vm->store->code + ht_jump_if_false_jump;}
                break;
            case OP_JUMP_IF_TRUE:
                value_t ht_jump_if_true = vm_stack_pop(vm);
                size_t ht_jump_if_true_jump = ADVANCE_INSTRUCTION_POINTER();
                if(ht_jump_if_true.number != 0) {vm->instruction_pointer = vm->store->code + ht_jump_if_true_jump;}
                break;
            case OP_JUMP:
                size_t ht_jump = ADVANCE_INSTRUCTION_POINTER();
                vm->instruction_pointer = vm->store->code + ht_jump;
                break;
            case OP_GOTO:
                size_t ht_goto = ADVANCE_INSTRUCTION_POINTER();
                vm->instruction_pointer = vm->store->code + ht_goto;
                break;
            case OP_REWIND:
                size_t ht_rewind = ADVANCE_INSTRUCTION_POINTER();
                vm->stack_top -= ht_rewind;
                break;
            case OP_SWITCH:
                vm->instruction_pointer = vm->store->code + h_switch_tables_list_solve(vm->switch_tables_list, ADVANCE_INSTRUCTION_POINTER(), vm_stack_pop(vm));
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
        case H_VALUE_CHAR:
            DEBUG_LOG("[%s, %c]\n", "Char", value->character);
            break;
        case H_VALUE_ARRAY:
            DEBUG_LOG("[%s<%s>, [", resolve_type(value), resolve_value_type(value->array->type));
            h_array_print_no_newline(value->array);
            DEBUG_LOG("]]\n");
            break;
        case H_VALUE_FUNCTION:
            DEBUG_LOG("["); 
            h_function_print_no_newline(value->function);
            DEBUG_LOG(", %s]\n", value->function->name->string); 
            break;
        case H_VALUE_NULL:
            DEBUG_LOG("[%s, %d]\n", "Null", 0);
            break;
        case H_VALUE_TYPE:
            DEBUG_LOG("[%s] - ", value->data_type->type_name->string);
            h_data_print_no_newline(value->data_type);
            DEBUG_LOG("\n");
            break;
        case H_VALUE_NATIVE:
            DEBUG_LOG("["); 
            h_native_print_no_newline(value->native_fn);
            DEBUG_LOG(", %s]\n", value->native_fn->name->string); 
            break;
        default:
            DEBUG_LOG("Undefined value\n");
            break;
    }
}

void vm_free(virtual_machine_t* vm) {
    free(vm->stack);
    free(vm);
}

#undef H_MAX_CALLS_STACK_SIZE
#undef H_VM_STACK_MIN_CAPACITY
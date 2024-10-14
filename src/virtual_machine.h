#ifndef H_VIRTUAL_MACHINE_H
#define H_VIRTUAL_MACHINE_H

#include "debug.h"
#include "bytecode_store.h"
#include "h_values.h"
#include "h_hash_table_t.h"
#include "h_locals_stack.h"

#define H_MAX_CALLS_STACK_SIZE 256

typedef struct call_frame_t {
  h_function_t* function;
  value_t* frame_stack;
  uint8_t* return_instruction;
} call_frame_t;

typedef struct virtual_machine_t {
    bytecode_store_t* store;
    bytecode_store_t* initial_store;
    uint8_t* instruction_pointer;
    value_t* stack;
    value_t* stack_top;
    value_t* stack_base;
    h_hash_table_t* globals_table;
    h_locals_stack_t* locals_stack;
    size_t calls_stack_size;
    call_frame_t calls_stack[H_MAX_CALLS_STACK_SIZE];
    value_t* array_initialisation_ptr;
} virtual_machine_t;

typedef enum interpreter_result_t {
  VM_SUCCESS,
  VM_ERROR,
} interpreter_result_t;

virtual_machine_t* vm_init(bytecode_store_t* store, h_hash_table_t* globals_table, h_locals_stack_t* locals_stack);
interpreter_result_t vm_run(virtual_machine_t* vm);
void vm_free(virtual_machine_t*);

#undef H_MAX_CALLS_STACK_SIZE

#endif
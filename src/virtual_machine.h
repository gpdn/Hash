#ifndef H_VIRTUAL_MACHINE_H
#define H_VIRTUAL_MACHINE_H

#include "debug.h"
#include "bytecode_store.h"
#include "h_values.h"
#include "h_hash_table_t.h"
#include "h_locals_stack.h"

typedef struct virtual_machine_t {
    bytecode_store_t* store;
    uint8_t* instruction_pointer;
    value_t* stack;
    value_t* stack_top;
    h_hash_table_t* globals_table;
    h_locals_stack_t* locals_stack;
} virtual_machine_t;

typedef enum interpreter_result_t {
  VM_SUCCESS,
  VM_ERROR,
} interpreter_result_t;

virtual_machine_t* vm_init(bytecode_store_t* store, h_hash_table_t* globals_table, h_locals_stack_t* locals_stack);
interpreter_result_t vm_run(virtual_machine_t* vm);
void vm_free(virtual_machine_t*);

#endif
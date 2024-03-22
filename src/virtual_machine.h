#ifndef H_VIRTUAL_MACHINE_H
#define H_VIRTUAL_MACHINE_H

#include "debug.h"
#include "bytecode_store.h"

typedef struct virtual_machine_t {
    bytecode_store_t* store;
    uint8_t* instruction_pointer;
} virtual_machine_t;

typedef enum interpreter_result_t {
  VM_SUCCESS,
  VM_ERROR,
} interpreter_result_t;

virtual_machine_t* vm_init(bytecode_store_t* store);
interpreter_result_t vm_run(virtual_machine_t* vm);
void vm_free(virtual_machine_t*);

#endif
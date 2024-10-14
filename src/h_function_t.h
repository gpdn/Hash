#ifndef HASH_FUNCTION_T_H
#define HASH_FUNCTION_T_H

#include "h_string_t.h"
#include "h_value_types.h"

typedef struct h_param_t {
    h_string_t* name;
    value_type_t type;
} h_param_t;

typedef struct h_function_t {
    h_string_t* name;
    h_param_t* parameters_list;
    size_t parameters_list_size;
    size_t parameters_list_capacity;
    value_type_t return_type;
    struct bytecode_store_t* store;
    struct h_locals_stack_t* locals_stack;
} h_function_t;

h_function_t* h_function_init();
h_function_t* h_function_init_set(h_string_t* name);
void h_function_parameter_add(h_function_t* function, h_string_t* name, value_type_t type);
void h_function_print(h_function_t* function);
void h_function_print_no_newline(h_function_t* function);
void h_function_free(h_function_t* function);

#endif
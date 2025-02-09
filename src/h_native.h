#ifndef H_NATIVE
#define H_NATIVE

#include "h_string_t.h"
#include "h_values.h"

typedef struct value_t (h_native_fn_t)(struct value_t* parameters, size_t args_count); 

typedef struct h_native_t {
    h_string_t* name;
    struct value_t* parameters_list_values;
    size_t parameters_list_size;
    size_t parameters_list_capacity;
    struct value_t* return_type;
    h_native_fn_t* fn;
} h_native_t;

h_native_t* h_native_init();
h_native_t* h_native_init_set(h_string_t* name, h_native_fn_t fn, struct value_t* params, size_t params_count, struct value_t return_type);
void h_native_parameter_add(h_native_t* function, struct value_t value);
void h_native_print(h_native_t* function);
void h_native_print_no_newline(h_native_t* function);
void h_native_free(h_native_t* function);

#endif
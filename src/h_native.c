#include "h_native.h"

#define H_DEFAULT_PARAMS_LIST_CAPACITY 5

h_native_t* h_native_init() {
    h_native_t* native = (h_native_t*)malloc(sizeof(h_native_t));
    native->name = NULL;
    native->parameters_list_values = (value_t*)malloc(sizeof(value_t) * H_DEFAULT_PARAMS_LIST_CAPACITY);
    native->parameters_list_size = 0;
    native->parameters_list_capacity = H_DEFAULT_PARAMS_LIST_CAPACITY;
    native->return_type = (value_t*)malloc(sizeof(value_t));
    native->return_type[0] = (value_t){.type = H_VALUE_UNDEFINED};
    native->fn = NULL;
    return native;
}

h_native_t* h_native_init_set(h_string_t* name, h_native_fn_t fn, value_t* params, size_t params_count, value_t return_type) {
    h_native_t* native = (h_native_t*)malloc(sizeof(h_native_t));
    native->name = name;
    native->parameters_list_values = (value_t*)malloc(sizeof(value_t) * params_count);
    memcpy(native->parameters_list_values, params, sizeof(value_t) * params_count);
    native->parameters_list_size = params_count;
    native->parameters_list_capacity = params_count;
    native->return_type = (value_t*)malloc(sizeof(value_t));
    native->return_type[0] = return_type;
    native->fn = fn;
    return native;
}

void h_native_parameter_add(h_native_t* function, struct value_t value) {
    if(function->parameters_list_size >= function->parameters_list_capacity) {
        function->parameters_list_capacity *= 2;
        function->parameters_list_values = (value_t*)realloc(function->parameters_list_values, sizeof(value_t) * function->parameters_list_capacity);
    }
    function->parameters_list_values[function->parameters_list_size++] = value;
}

void h_native_print(h_native_t* function) {
    if(!function) return;
    printf("fn[%s](", resolve_value_type(function->return_type->type));
    for(size_t i = 0; i < function->parameters_list_size; ++i) printf("%s", resolve_value_type(function->parameters_list_values[i].type));
    printf(")\n");
}

void h_native_print_no_newline(h_native_t* function) {
    if(!function) return;
    printf("fn[%s](", resolve_value_type(function->return_type->type));
    if(function->parameters_list_size > 0) {
        for(size_t i = 0; i < function->parameters_list_size - 1; ++i) printf("%s, ", resolve_value_type(function->parameters_list_values[i].type));
        printf("%s", resolve_value_type(function->parameters_list_values[function->parameters_list_size - 1].type));
    }
    printf(")");
}

void h_native_free(h_native_t* function) {
    if(function->parameters_list_values) free(function->parameters_list_values);
    free(function);
}

#undef H_DEFAULT_PARAMS_LIST_CAPACITY

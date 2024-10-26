#include "h_function_t.h"
#include "bytecode_store.h"
#include "h_locals_stack.h"

#define H_DEFAULT_PARAMS_LIST_CAPACITY 5
#define H_DEFAULT_BYTECODE_STORE_CAPACITY 20
#define H_DEFAULT_LOCALS_STACK_CAPACITY 20

h_function_t* h_function_init() {
    h_function_t* function = (h_function_t*)malloc(sizeof(h_function_t));
    function->name = NULL;
    function->parameters_list_size = 0;
    function->parameters_list_capacity = H_DEFAULT_PARAMS_LIST_CAPACITY;
    function->parameters_list_names = (h_string_t**)malloc(sizeof(h_string_t*) * H_DEFAULT_PARAMS_LIST_CAPACITY);
    function->parameters_list_values = (value_t*)malloc(sizeof(value_t) * H_DEFAULT_PARAMS_LIST_CAPACITY);
    function->return_type = (value_t*)malloc(sizeof(value_t) * H_DEFAULT_PARAMS_LIST_CAPACITY);
    function->return_type[0] = (value_t){.type = H_VALUE_UNDEFINED};
    function->store = bs_init(H_DEFAULT_BYTECODE_STORE_CAPACITY);
    function->locals_stack = h_locals_stack_init(H_DEFAULT_LOCALS_STACK_CAPACITY);
    return function;
}

void h_function_parameter_add(h_function_t* function, h_string_t* name, value_t value) {
    if(function->parameters_list_size >= function->parameters_list_capacity) {
        function->parameters_list_capacity *= 2;
        function->parameters_list_names = (h_string_t**)realloc(function->parameters_list_names, sizeof(h_string_t*) * function->parameters_list_capacity);
        function->parameters_list_values = (value_t*)realloc(function->parameters_list_values, sizeof(value_t) * function->parameters_list_capacity);
    }
    function->parameters_list_names[function->parameters_list_size] = name;
    function->parameters_list_values[function->parameters_list_size++] = value;
    h_locals_stack_push(function->locals_stack, name, value, 0);
}

void h_function_print(h_function_t* function) {
    if(!function) return;
    printf("fn[%s](", resolve_value_type(function->return_type->type));
    for(size_t i = 0; i < function->parameters_list_size; ++i) printf("%s %s", resolve_value_type(function->parameters_list_values[i].type), function->parameters_list_names[i]->string);
    printf(")\n");
}

void h_function_print_no_newline(h_function_t* function) {
    if(!function) return;
    printf("fn[%s](", resolve_value_type(function->return_type->type));
    if(function->parameters_list_size > 0) {
        for(size_t i = 0; i < function->parameters_list_size - 1; ++i) printf("%s %s, ", resolve_value_type(function->parameters_list_values[i].type), function->parameters_list_names[i]->string);
        printf("%s %s", resolve_value_type(function->parameters_list_values[function->parameters_list_size - 1].type), function->parameters_list_names[function->parameters_list_size - 1]->string);
    }
    printf(")");
}

void h_function_free(h_function_t* function) {
    if(function->parameters_list_names) free(function->parameters_list_names);
    if(function->parameters_list_values) free(function->parameters_list_values);
    h_locals_stack_free(function->locals_stack);
    bs_free(function->store);
    free(function);
}

#undef H_DEFAULT_PARAMS_LIST_CAPACITY
#undef H_DEFAULT_BYTECODE_STORE_CAPACITY
#undef H_DEFAULT_LOCALS_STACK_CAPACITY
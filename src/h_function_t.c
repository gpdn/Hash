#include "h_function_t.h"

#define H_DEFAULT_PARAMS_LIST_CAPACITY 5

h_function_t* h_function_init() {
    h_function_t* function = (h_function_t*)malloc(sizeof(h_function_t));
    function->name = NULL;
    function->parameters_list_size = 0;
    function->parameters_list_capacity = H_DEFAULT_PARAMS_LIST_CAPACITY;
    function->parameters_list = (h_param_t*)malloc(sizeof(h_param_t) * H_DEFAULT_PARAMS_LIST_CAPACITY);
    function->return_type = H_VALUE_UNDEFINED;
    return function;
}

void h_function_parameter_add(h_function_t* function, h_string_t* name, value_type_t type) {
    if(function->parameters_list_size >= function->parameters_list_capacity) {
        function->parameters_list_capacity *= 2;
        function->parameters_list = (h_param_t*)realloc(function->parameters_list, sizeof(h_param_t) * function->parameters_list_capacity);
    }
    function->parameters_list[function->parameters_list_size++] = (h_param_t){name, type};
}

void h_function_print(h_function_t* function) {
    printf("fn[%s](", resolve_value_type(function->return_type));
    for(size_t i = 0; i < function->parameters_list_size; ++i) printf("%s %s", resolve_value_type(function->parameters_list[i].type), function->parameters_list[i].name->string);
    printf(")\n");
}

void h_function_print_no_newline(h_function_t* function) {
    printf("fn[%s](", resolve_value_type(function->return_type));
    for(size_t i = 0; i < function->parameters_list_size - 1; ++i) printf("%s %s, ", resolve_value_type(function->parameters_list[i].type), function->parameters_list[i].name->string);
    printf("%s %s", resolve_value_type(function->parameters_list[function->parameters_list_size - 1].type), function->parameters_list[function->parameters_list_size - 1].name->string);
    printf(")");
}

void h_function_free(h_function_t* function) {
    if(function->parameters_list) free(function->parameters_list);
    free(function);
}

#undef H_DEFAULT_PARAMS_LIST_CAPACITY
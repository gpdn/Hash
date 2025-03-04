#include "h_preprocessor_function.h"

#define H_PREPROCESSOR_FUNCTION_DEFAULT_CAPACITY 5

h_preprocessor_function_t* h_preprocessor_function_init(h_string_t* name) {
    h_preprocessor_function_t* function = (h_preprocessor_function_t*)malloc(sizeof(h_preprocessor_function_t));
    function->name = name;
    function->capacity = H_PREPROCESSOR_FUNCTION_DEFAULT_CAPACITY;
    function->parameters_list = (h_string_t**)malloc(sizeof(h_string_t*) * function->capacity);
    function->params_count = 0;
    function->body = NULL;
    return function;
}

int h_preprocessor_function_append(h_preprocessor_function_t* function, h_string_t* param) {
    if(function->params_count >= function->capacity) {
        function->capacity = function->capacity << 2;
        function->parameters_list = (h_string_t**)realloc(function->parameters_list, sizeof(h_string_t*) * function->capacity);
    }
    function->parameters_list[function->params_count++] = param;
    return 1;
}

int h_preprocessor_function_set_body(h_preprocessor_function_t* function, h_string_t* body) {
    function->body = body;
    return 1;
}

void h_preprocessor_function_free(h_preprocessor_function_t* function) {
    for(size_t i = 0; i < function->params_count; ++i) {
        h_string_free(function->parameters_list[i]);
    }
    free(function->parameters_list);
    free(function);
}


#undef H_PREPROCESSOR_FUNCTION_DEFAULT_CAPACITY
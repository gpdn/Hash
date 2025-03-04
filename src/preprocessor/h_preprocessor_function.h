#ifndef H_PREPROCESSOR_FUNCTION
#define H_PREPROCESSOR_FUNCTION

#include <stdlib.h> 
#include "../h_string_t.h"

typedef struct h_preprocessor_function_t {
    h_string_t* name;
    h_string_t** parameters_list;
    size_t params_count;
    size_t capacity;
    h_string_t* body;
} h_preprocessor_function_t;

h_preprocessor_function_t* h_preprocessor_function_init(h_string_t* name);
int h_preprocessor_function_append(h_preprocessor_function_t* function, h_string_t* param);
int h_preprocessor_function_set_body(h_preprocessor_function_t* function, h_string_t* body);
void h_preprocessor_function_free(h_preprocessor_function_t* function);

#endif
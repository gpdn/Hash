#ifndef H_PREPROCESSOR_VALUE
#define H_PREPROCESSOR_VALUE

#include "../h_string_t.h"
#include "h_preprocessor_function.h"

typedef enum h_preprocessor_value_type_t {
    H_PREPROCESSOR_VALUE_UNDEFINED,
    H_PREPROCESSOR_VALUE_INT,
    H_PREPROCESSOR_VALUE_STRING,
    H_PREPROCESSOR_VALUE_IDENTIFIER,
    H_PREPROCESSOR_VALUE_FUNCTION
} h_preprocessor_value_type_t;

typedef struct h_preprocessor_value_t {
    union {
        int number;
        h_string_t* string;
        h_preprocessor_function_t* function;
    };
    h_preprocessor_value_type_t type;
} h_preprocessor_value_t;

#define PREPROCESSOR_VALUE_UNDEFINED() ((h_preprocessor_value_t){.number = 0, .type = H_PREPROCESSOR_VALUE_UNDEFINED})
#define PREPROCESSOR_VALUE_NUM(value) ((h_preprocessor_value_t){.number = value, .type = H_PREPROCESSOR_VALUE_INT})
#define PREPROCESSOR_VALUE_STRING(value) ((h_preprocessor_value_t){.string = value, .type = H_PREPROCESSOR_VALUE_STRING})
#define PREPROCESSOR_VALUE_IDENTIFIER(value) ((h_preprocessor_value_t){.string = value, .type = H_PREPROCESSOR_VALUE_IDENTIFIER})
#define PREPROCESSOR_VALUE_FUNCTION(value) ((h_preprocessor_value_t){.function = value, .type = H_PREPROCESSOR_VALUE_FUNCTION})

void preprocessor_print_value(h_preprocessor_value_t value);

#endif
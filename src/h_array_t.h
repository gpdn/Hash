#ifndef H_ARRAY_T
#define H_ARRAY_T

#include <stdlib.h>
#include <stdint.h>
#include "h_value_types.h"
#include "h_values.h"

typedef struct h_array_t {
    size_t size;
    size_t capacity;
    struct value_t* data;
    value_type_t type;
} h_array_t;

h_array_t* h_array_init(value_type_t type, size_t capacity);
void h_array_push(h_array_t* array, struct value_t value);
struct value_t h_array_get(h_array_t* array, size_t index);
void h_array_set(h_array_t* array, size_t index, struct value_t value);
void h_array_print(h_array_t* array);
void h_array_print_no_newline(h_array_t* array);
void h_array_free(h_array_t* array);

#endif
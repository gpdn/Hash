#ifndef H_DATA_T
#define H_DATA_T

#include <stdlib.h>
#include <stdint.h>
#include "h_value_types.h"
#include "h_values.h"

typedef struct h_data_t {
    size_t size;
    size_t capacity;
    struct value_t* data;
    h_string_t* type_name;
    void* other;
} h_data_t;

h_data_t* h_data_init(size_t capacity);
void h_data_push(h_data_t* data, struct value_t value);
struct value_t h_data_get(h_data_t* data, size_t index);
void h_data_set(h_data_t* data, size_t index, struct value_t value);
void h_data_print(h_data_t* data);
void h_data_print_no_newline(h_data_t* data);
void h_data_free(h_data_t* data);

#endif
#ifndef HASH_STRUCT_T
#define HASH_STRUCT_T

#include "h_value_types.h"
#include "h_values.h"

typedef struct h_struct_t {
    h_string_t* name;
    h_string_t* field_names;
    struct value_t* field_values;
    size_t size;
    size_t capacity;
} h_struct_t;

h_struct_t* h_struct_init(h_string_t* name);
void h_struct_field_add(h_struct_t* data, h_string_t* name, struct value_t value);
int h_struct_field_get_index(h_struct_t* data, h_string_t* name);
void h_struct_print(h_struct_t* data);
void h_struct_free(h_struct_t* data);

#endif
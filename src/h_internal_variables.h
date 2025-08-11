#ifndef H_INTERNAL_VARIABLES
#define H_INTERNAL_VARIABLES

#include "h_values.h"

typedef enum h_internal_variable_tag_t {
    H_INTERNAL_TRACE,
    H_INTERNAL_LAST,
} h_internal_variable_tag_t;

typedef struct h_internal_variable_t {
    h_internal_variable_tag_t tag;
    h_string_t* name;
    value_t value;
} h_internal_variable_t;

void h_internal_variables_init();
h_internal_variable_t h_internal_variables_resolve(h_string_t* string);
h_internal_variable_t h_internal_variables_resolve_tag(h_internal_variable_tag_t tag);
value_t h_internal_variables_resolve_get(h_internal_variable_tag_t tag);
void h_internal_variables_free();

#endif
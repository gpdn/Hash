#ifndef H_GENERIC_ARGUMENTS_LIST
#define H_GENERIC_ARGUMENTS_LIST

#include <stdlib.h>
#include "debug.h"
#include "h_values.h"

typedef struct h_generic_arguments_list_t {
    value_t* arguments;
    size_t size;
    size_t capacity;
} h_generic_arguments_list_t;

h_generic_arguments_list_t* h_generic_arguments_list_init();
void h_generic_arguments_list_add(h_generic_arguments_list_t* list, value_t value);
int h_generic_arguments_list_get_index(h_generic_arguments_list_t* list, value_t value);
void h_generic_arguments_list_print(h_generic_arguments_list_t* list);
void h_generic_arguments_list_free(h_generic_arguments_list_t* list);

#endif
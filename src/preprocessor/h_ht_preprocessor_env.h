#ifndef H_PREPROCESSOR_ENV
#define H_PREPROCESSOR_ENV

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../debug.h"
#include "../h_string_t.h"
#include "h_preprocessor_value.h"

typedef struct h_preprocessor_env_entry_t {
    h_string_t* name;
    h_preprocessor_value_t value;
} h_preprocessor_env_entry_t;

typedef struct h_preprocessor_env_t {
    size_t elements_count;
    size_t capacity;
    h_preprocessor_env_entry_t* array;
    float load_factor;
} h_preprocessor_env_t;

h_preprocessor_env_t* h_preprocessor_env_init(size_t capacity, float load_factor);
int h_preprocessor_env_define(h_preprocessor_env_t* table, h_string_t* key, h_preprocessor_value_t value);
int h_preprocessor_env_set(h_preprocessor_env_t* table, h_string_t* key, h_preprocessor_value_t value);
int h_preprocessor_env_unset(h_preprocessor_env_t* table, h_string_t* key);
h_preprocessor_value_t h_preprocessor_env_get(h_preprocessor_env_t* table, h_string_t* key);
void h_preprocessor_env_free(h_preprocessor_env_t* hash_table);

#endif
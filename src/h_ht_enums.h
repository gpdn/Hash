#ifndef H_HT_ENUMS_H
#define H_HT_ENUMS_H

#include <stdlib.h>
#include <stdint.h>
#include "debug.h"
#include "h_values.h"
#include "string.h"

/* typedef struct ht_enum_values_t {
    h_string_t** names;
    size_t size;
    size_t capacity;
} ht_enum_values_t; */

typedef struct ht_enum_t {
    h_string_t* name;
    h_string_t** names;
    size_t size;
    size_t capacity;
} ht_enum_t;

typedef struct h_ht_enums_t {
    size_t elements_count;
    size_t capacity;
    ht_enum_t* array;
    float load_factor;
} h_ht_enums_t;

h_ht_enums_t* h_ht_enums_init(size_t capacity, float load_factor);
void h_ht_enums_free(h_ht_enums_t* hash_table);
void h_ht_enums_print(h_ht_enums_t* table);
ht_enum_t* h_ht_enums_get(h_ht_enums_t* table, h_string_t* key);
ht_enum_t* h_ht_enums_set(h_ht_enums_t* table, h_string_t* key);
void h_ht_enum_value_set(ht_enum_t* array, h_string_t* key);
int h_ht_enum_value_get(ht_enum_t* array, h_string_t* key);

#endif
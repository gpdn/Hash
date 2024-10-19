#ifndef H_HT_TYPES_H
#define H_HT_TYPES_H

#include <stdlib.h>
#include <stdint.h>
#include "debug.h"
#include "h_values.h"
#include "string.h"

typedef enum ht_type_info_t {
    H_TYPE_INFO_UNDEFINED,
    H_TYPE_INFO_DATA,
    H_TYPE_INFO_ALIAS
} ht_type_info_t;

typedef struct ht_type_t {
    ht_type_info_t type;
    h_string_t* name;
    value_t value;
} ht_type_t;

typedef struct h_ht_types_t {
    size_t elements_count;
    size_t capacity;
    ht_type_t* array;
    float load_factor;
} h_ht_types_t;

h_ht_types_t* h_ht_types_init(size_t capacity, float load_factor);
void h_ht_types_free(h_ht_types_t* hash_table);
void h_ht_types_print(h_ht_types_t* table);
int h_ht_types_check_defined(h_ht_types_t* table, h_string_t* key);
ht_type_t h_ht_types_get(h_ht_types_t* table, h_string_t* key);
int h_ht_types_set(h_ht_types_t* table, h_string_t* key, value_t value, ht_type_info_t type);
size_t h_ht_types_get_index(h_ht_types_t* table, h_string_t* key);
size_t h_ht_types_array_get(h_ht_types_t* table, size_t index);

#endif
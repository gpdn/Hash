#ifndef H_HASH_TABLE_H
#define H_HASH_TABLE_H

#include <stdlib.h>
#include <stdint.h>
#include "debug.h"
#include "h_values.h"
#include "string.h"

typedef struct ht_entry_t {
    h_string_t* name;
    value_t value;
} ht_entry_t;

typedef struct h_hash_table_t {
    size_t elements_count;
    size_t capacity;
    ht_entry_t* array;
    float load_factor;
} h_hash_table_t;

h_hash_table_t* h_hash_table_init(size_t capacity, float load_factor);
void h_hash_table_free(h_hash_table_t* hash_table);
void h_ht_print(h_hash_table_t* table);
value_t h_ht_get(h_hash_table_t* table, h_string_t* key);
int h_ht_set(h_hash_table_t* table, h_string_t* key, value_t value);
value_t h_ht_increase(h_hash_table_t* table, h_string_t* key);
value_t h_ht_decrease(h_hash_table_t* table, h_string_t* key);

#endif
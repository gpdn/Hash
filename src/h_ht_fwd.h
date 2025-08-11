#ifndef H_HT_fwd_H
#define H_HT_fwd_H

#include <stdlib.h>
#include <stdint.h>
#include "debug.h"
#include "h_values.h"
#include "string.h"

typedef struct ht_fwd_entry_t {
    h_string_t* name;
    size_t index;
    int64_t next;
} ht_fwd_entry_t;

typedef struct ht_fwd_dense_t {
    h_string_t* name;
    uint8_t defined;
} ht_fwd_dense_t;

typedef struct h_ht_fwd_t {
    size_t size;
    size_t capacity;
    ht_fwd_entry_t* sparse_array;
    ht_fwd_dense_t* dense_array;
    float load_factor;
} h_ht_fwd_t;

h_ht_fwd_t* h_ht_fwd_init(size_t capacity, float load_factor);
void h_ht_fwd_free(h_ht_fwd_t* hash_table);
void h_ht_fwd_print(h_ht_fwd_t* table);
int h_ht_fwd_check_defined(h_ht_fwd_t* table, h_string_t* key);
int h_ht_fwd_set(h_ht_fwd_t* table, h_string_t* key);
int h_ht_fwd_define(h_ht_fwd_t* table, h_string_t* key);

#endif
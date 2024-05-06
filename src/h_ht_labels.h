#ifndef H_HT_LABELS_H
#define H_HT_LABELS_H

#include <stdlib.h>
#include <stdint.h>
#include "debug.h"
#include "h_values.h"
#include "string.h"

typedef struct ht_label_t {
    h_string_t* name;
    size_t index;
} ht_label_t;

typedef struct h_ht_labels_t {
    size_t elements_count;
    size_t capacity;
    ht_label_t* array;
    float load_factor;
} h_ht_labels_t;

h_ht_labels_t* h_ht_labels_init(size_t capacity, float load_factor);
void h_ht_labels_free(h_ht_labels_t* hash_table);
void h_ht_labels_print(h_ht_labels_t* table);
size_t h_ht_labels_get(h_ht_labels_t* table, h_string_t* key);
int h_ht_labels_set(h_ht_labels_t* table, h_string_t* key, size_t index);
size_t h_ht_labels_get_index(h_ht_labels_t* table, h_string_t* key);
size_t h_ht_labels_array_get(h_ht_labels_t* table, size_t index);

#endif
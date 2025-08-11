#include "h_ht_fwd.h"

h_ht_fwd_t* h_ht_fwd_init(size_t capacity, float load_factor) {
    h_ht_fwd_t* table = (h_ht_fwd_t*)malloc(sizeof(h_ht_fwd_t));
    table->capacity = capacity;
    table->load_factor = load_factor;
    table->size = 0;
    table->sparse_array = (ht_fwd_entry_t*)calloc(capacity, sizeof(ht_fwd_entry_t));
    table->dense_array = (ht_fwd_dense_t*)calloc(capacity, sizeof(ht_fwd_dense_t));
    return table;
}

void h_ht_fwd_free(h_ht_fwd_t* table) {
    free(table->sparse_array);
    free(table->dense_array);
    free(table);
}

void h_ht_fwd_print(h_ht_fwd_t* table) {
    for(size_t i = 0; i < table->size; ++i) {
        DEBUG_LOG("[%lld] - ", i);
        DEBUG_LOG("%s, %d\n", table->dense_array[i].name->string, table->dense_array[i].defined);
    }
}

inline int h_ht_fwd_check_defined(h_ht_fwd_t* table, h_string_t* key) {
    return table->sparse_array[key->hash & (table->capacity - 1)].name != NULL;
}

inline int h_ht_fwd_set(h_ht_fwd_t* table, h_string_t* key) {
    if (table->size >= table->capacity) {
        table->capacity <<= 1;
        table->dense_array = (ht_fwd_dense_t*)realloc(table->dense_array, sizeof(ht_fwd_dense_t) * table->capacity);
    }
    table->sparse_array[key->hash & (table->capacity - 1)] = (ht_fwd_entry_t){.name = key, .index = table->size, .next = -1};
    table->dense_array[table->size++] = (ht_fwd_dense_t){.name = key, .defined = 0};
    return 1;
}

inline int h_ht_fwd_define(h_ht_fwd_t* table, h_string_t* key) {
    ht_fwd_entry_t entry = table->sparse_array[key->hash & (table->capacity - 1)];
    if(entry.name == NULL) return 0;
    table->dense_array[entry.index].defined = 1;
    return 1;
}
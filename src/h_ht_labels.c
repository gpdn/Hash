#include "h_ht_labels.h"

h_ht_labels_t* h_ht_labels_init(size_t capacity, float load_factor) {
    h_ht_labels_t* table = (h_ht_labels_t*)malloc(sizeof(h_ht_labels_t));
    table->capacity = capacity;
    table->load_factor = load_factor;
    table->elements_count = 0;
    table->array = (ht_label_t*)calloc(capacity, sizeof(ht_label_t));
    return table;
}

void h_ht_labels_print(h_ht_labels_t* table) {
    for(size_t i = 0; i < table->capacity; ++i) {
        DEBUG_LOG("[%lld] - ", i);
        if(table->array[i].name == NULL) {DEBUG_LOG("Empty\n"); continue;}
        DEBUG_LOG("%s, %lld\n", table->array[i].name->string, table->array[i].index);
    }
}

size_t h_ht_labels_get(h_ht_labels_t* table, h_string_t* key) {
    return table->array[key->hash & (table->capacity - 1)].index;
}

int h_ht_labels_find(h_ht_labels_t* table, h_string_t* key) {
    return table->array[key->hash & (table->capacity - 1)].name != NULL;
}

int h_ht_labels_set(h_ht_labels_t* table, h_string_t* key, size_t index) {
    table->array[key->hash & (table->capacity - 1)] = (ht_label_t){key, index};
    DEBUG_LOG("Label Index set to: %lld\n", index);
    return 1;
}

size_t h_ht_labels_get_index(h_ht_labels_t* table, h_string_t* key) {
    return (size_t)(key->hash & (table->capacity - 1));
}

size_t h_ht_labels_array_get(h_ht_labels_t* table, size_t index) {
    return table->array[index].index;
}


void h_ht_labels_free(h_ht_labels_t* table) {
    free(table->array);
    free(table);
}
#include "h_hash_table_t.h"

h_hash_table_t* h_hash_table_init(size_t capacity, float load_factor) {
    h_hash_table_t* hash_table = (h_hash_table_t*)malloc(sizeof(h_hash_table_t));
    hash_table->capacity = capacity;
    hash_table->load_factor = load_factor;
    hash_table->elements_count = 0;
    hash_table->array = (ht_entry_t*)calloc(capacity, sizeof(ht_entry_t));
    return hash_table;
}

int h_ht_set(h_hash_table_t* table, h_string_t* key, value_t value) {
    size_t index = (size_t)key->hash % table->capacity;
    ht_entry_t* entry = table->array + index;
    if(entry->name == NULL) {
        ++table->elements_count;
    }
    entry->name = key;
    entry->value = value;
    return 1;
}

value_t h_ht_increase(h_hash_table_t* table, h_string_t* key) {
    size_t index = (size_t)key->hash % table->capacity;
    ht_entry_t* entry = table->array + index;
    ++entry->value.number;
    return entry->value;
}

value_t h_ht_decrease(h_hash_table_t* table, h_string_t* key) {
    size_t index = (size_t)key->hash % table->capacity;
    ht_entry_t* entry = table->array + index;
    --entry->value.number;
    return entry->value;
}

value_t h_ht_get(h_hash_table_t* table, h_string_t* key) {
    ht_entry_t* entry = &table->array[(size_t)(key->hash % table->capacity)];
    return entry->value;
};

size_t h_ht_get_index(h_hash_table_t* table, h_string_t* key) {
    return (size_t)(key->hash % table->capacity);
};

value_t h_ht_array_get(h_hash_table_t* table, size_t index) {
    return table->array[index].value;
};

void h_ht_array_set(h_hash_table_t* table, size_t index, value_t value) {
    table->array[index].value = value;
}

value_t h_ht_array_increase(h_hash_table_t* table, size_t index) {
    ++table->array[index].value.number;
    return table->array[index].value;
}

value_t h_ht_array_decrease(h_hash_table_t* table, size_t index) {
    --table->array[index].value.number;
    return table->array[index].value;
}

void h_ht_print(h_hash_table_t* table) {
    for(size_t i = 0; i < table->capacity; ++i) {
        DEBUG_LOG("[%lld] - ", i);
        if(table->array[i].name == NULL) {DEBUG_LOG("Empty\n"); continue;}
        DEBUG_LOG("%s, ", table->array[i].name->string);
        #if DEBUG_ALL
            print_value(&table->array[i].value);
        #endif
    }
}

void h_hash_table_free(h_hash_table_t* hash_table) {
    free(hash_table->array);
    free(hash_table);
}
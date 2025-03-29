#include "h_ht_preprocessor_env.h"

h_preprocessor_env_t* h_preprocessor_env_init(size_t capacity, float load_factor) {
    h_preprocessor_env_t* hash_table = (h_preprocessor_env_t*)malloc(sizeof(h_preprocessor_env_t));
    hash_table->capacity = capacity;
    hash_table->load_factor = load_factor;
    hash_table->elements_count = 0;
    hash_table->array = (h_preprocessor_env_entry_t*)calloc(capacity, sizeof(h_preprocessor_env_entry_t));
    return hash_table;
}

int h_preprocessor_env_define(h_preprocessor_env_t* table, h_string_t* key, h_preprocessor_value_t value) {
    size_t index = key->hash & (table->capacity - 1);
    if(table->array[index].value.type != H_PREPROCESSOR_VALUE_UNDEFINED) return 0;
    table->array[index] = (h_preprocessor_env_entry_t){.name = key, .value = value};
    ++table->elements_count;
    return 1;
}

int h_preprocessor_env_set(h_preprocessor_env_t* table, h_string_t* key, h_preprocessor_value_t value) {
    size_t index = key->hash & (table->capacity - 1);
    if(table->array[index].value.type == H_PREPROCESSOR_VALUE_UNDEFINED) return 0;
    table->array[index] = (h_preprocessor_env_entry_t){.name = key, .value = value};
    ++table->elements_count;
    return 1;
}

int h_preprocessor_env_unset(h_preprocessor_env_t* table, h_string_t* key) {
    size_t index = key->hash & (table->capacity - 1);
    if(table->array[index].value.type == H_PREPROCESSOR_VALUE_UNDEFINED) return 0;
    table->array[index] = (h_preprocessor_env_entry_t){.name = key, .value = PREPROCESSOR_VALUE_UNDEFINED()};
    --table->elements_count;
    return 1;
}

h_preprocessor_value_t h_preprocessor_env_get(h_preprocessor_env_t* table, h_string_t* key) {
    size_t index = key->hash & (table->capacity - 1);
    return table->array[index].value;
}

void h_preprocessor_env_free(h_preprocessor_env_t* hash_table) {
    free(hash_table->array);
    free(hash_table);
}
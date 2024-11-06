#include "h_ht_files_set.h"

static size_t h_files_set_hash(const char* string);

static size_t h_files_set_hash(const char* string) {
    uint32_t hash = 2166136261u;
    size_t length = strlen(string);
    for(const char* temp = string; temp != string + length; ++temp) {
        hash ^= (uint8_t)*temp;
        hash *= 16777619;
    }
    return hash;
}

h_files_set_t* h_files_set_init(size_t capacity, float load_factor) {
    h_files_set_t* hash_table = (h_files_set_t*)malloc(sizeof(h_files_set_t));
    hash_table->capacity = capacity;
    hash_table->load_factor = load_factor;
    hash_table->elements_count = 0;
    hash_table->array = (int*)calloc(capacity, sizeof(int));
    return hash_table;
}

int h_files_set_push(h_files_set_t* table, const char* key) {
    size_t index = h_files_set_hash(key) % table->capacity;
    if(table->array[index] == 1) return 0;
    table->array[index] = 1;
    ++table->elements_count;
    return 1;
}

void h_files_set_free(h_files_set_t* hash_table) {
    free(hash_table->array);
    free(hash_table);
}
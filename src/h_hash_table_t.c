#include "h_hash_table_t.h"

static size_t h_ht_hash(const char* string);

static size_t h_ht_hash(const char* string) {
    uint32_t hash = 2166136261u;
    for(const char* temp = string; *temp != '\0'; ++temp) {
        hash ^= (uint8_t)*temp;
        hash *= 16777619;
    }
    return hash;
}

h_hash_table_t* h_hash_table_init(size_t capacity, float load_factor) {
    h_hash_table_t* hash_table = (h_hash_table_t*)malloc(sizeof(h_hash_table_t));
    hash_table->capacity = capacity;
    hash_table->load_factor = load_factor;
    hash_table->elements_count = 0;
    hash_table->array = (ht_entry_t*)calloc(capacity, sizeof(ht_entry_t));
    return hash_table;
}

int h_ht_set(h_hash_table_t* table, const char* key, value_t value) {
    size_t index = (size_t)h_ht_hash(key) % table->capacity;
    ht_entry_t* entry = table->array + index;
    if(entry->name == NULL) {
        ++table->elements_count;
    }
    entry->name = key;
    entry->value = value;
    return 1;
}

value_t h_ht_get(h_hash_table_t* table, const char* key) {
    return table->array[h_ht_hash(key) % table->capacity].value;
};

void h_ht_print(h_hash_table_t* table) {
    for(size_t i = 0; i < table->capacity; ++i) {
        DEBUG_LOG("[%lld] - ", i);
        if(table->array[i].name == NULL) {DEBUG_LOG("Empty\n"); continue;}
        DEBUG_LOG("%s, %f\n", table->array[i].name, table->array[i].value); 
    }
}

void h_hash_table_free(h_hash_table_t* hash_table) {
    free(hash_table->array);
    free(hash_table);
}
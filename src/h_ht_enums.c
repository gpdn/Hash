#include "h_ht_enums.h"

#define H_ENUM_ARRAY_INITIAL_SIZE 20

/* static ht_enum_values_t* h_ht_enum_values_init(size_t capacity);

static ht_enum_values_t* h_ht_enum_values_init(size_t capacity) {
    ht_enum_values_t* new_enum = (ht_enum_values_t*)malloc(sizeof(ht_enum_values_t));
    new_enum->capacity = capacity;
    new_enum->size = 0;
    new_enum->names = (h_string_t**)calloc(capacity, sizeof(h_string_t*));
    return new_enum;
} */

h_ht_enums_t* h_ht_enums_init(size_t capacity, float load_factor) {
    h_ht_enums_t* table = (h_ht_enums_t*)malloc(sizeof(h_ht_enums_t));
    table->capacity = capacity;
    table->load_factor = load_factor;
    table->elements_count = 0;
    table->array = (ht_enum_t*)calloc(capacity, sizeof(ht_enum_t));
    return table;
}

void h_ht_enums_print(h_ht_enums_t* table) {
    for(size_t i = 0; i < table->capacity; ++i) {
        DEBUG_LOG("[%lld] - ", i);
        if(table->array[i].name == NULL) {DEBUG_LOG("Empty\n"); continue;}
        DEBUG_LOG("[");
        for(size_t j = 0; j < table->array[i].size; ++j) {
            DEBUG_LOG("%s,", table->array[i].names[j]->string);
        }
        DEBUG_LOG("]\n");
    }
}

ht_enum_t* h_ht_enums_get(h_ht_enums_t* table, h_string_t* key) {
    return &table->array[key->hash % table->capacity];
}


ht_enum_t* h_ht_enums_set(h_ht_enums_t* table, h_string_t* key) {
    size_t index = key->hash % table->capacity; 
    table->array[index] = (ht_enum_t){.name=key, .size=0, .capacity=H_ENUM_ARRAY_INITIAL_SIZE, .names=(h_string_t**)calloc(H_ENUM_ARRAY_INITIAL_SIZE, sizeof(h_string_t*))};
    return table->array + index;
}

void h_ht_enum_value_set(ht_enum_t* array, h_string_t* key) {
    if(array->size >= array->capacity) {
        array->capacity *= 2;
        array->names = (h_string_t**)realloc(array->names, sizeof(h_string_t*) * array->capacity);
    }
    array->names[array->size++] = key;
}

int h_ht_enum_value_get(ht_enum_t* array, h_string_t* key) {
    DEBUG_LOG("Size: %lld", array->size);
    for(size_t i = 0; i < array->size; ++i) {
        if(array->names[i]->hash == key->hash) return (int)i;
    }
    return -1;
}

void h_ht_enums_free(h_ht_enums_t* table) {
    for(size_t i = 0; i < table->elements_count; ++i) {
        if(table->array[i].name) {
            free(table->array[i].names);
        }
    }
    free(table->array);
    free(table);
}

#undef H_ENUM_ARRAY_INITIAL_SIZE
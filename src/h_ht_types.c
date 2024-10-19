#include "debug.h"
#include "h_ht_types.h"
#include "h_values.h"

h_ht_types_t* h_ht_types_init(size_t capacity, float load_factor) {
    h_ht_types_t* table = (h_ht_types_t*)malloc(sizeof(h_ht_types_t));
    table->capacity = capacity;
    table->load_factor = load_factor;
    table->elements_count = 0;
    table->array = (ht_type_t*)calloc(capacity, sizeof(ht_type_t));
    return table;
}

void h_ht_types_print(h_ht_types_t* table) {
    for(size_t i = 0; i < table->capacity; ++i) {
        DEBUG_LOG("[%lld] - ", i);
        if(table->array[i].name == NULL) {DEBUG_LOG("Empty\n"); continue;}
        DEBUG_LOG("%s - ", table->array[i].name->string);
        print_value(&table->array[i].value);
    }
}

int h_ht_types_check_defined(h_ht_types_t* table, h_string_t* key) {
    return table->array[key->hash % table->capacity].type != H_TYPE_INFO_UNDEFINED;
}

ht_type_t h_ht_types_get(h_ht_types_t* table, h_string_t* key) {
    return table->array[key->hash % table->capacity];
}

int h_ht_types_set(h_ht_types_t* table, h_string_t* key, value_t value, ht_type_info_t type) {
    table->array[key->hash % table->capacity] = (ht_type_t){type, key, value};
    DEBUG_LOG("New type set: %s\n", key->string);
    return 1;
}

size_t h_ht_types_get_index(h_ht_types_t* table, h_string_t* key) {
    return (size_t)key->hash % table->capacity;
}

size_t h_ht_types_array_get(h_ht_types_t* table, size_t index) {
    return table->array[index].type;
}

void h_ht_types_free(h_ht_types_t* table) {
    free(table->array);
    free(table);
}
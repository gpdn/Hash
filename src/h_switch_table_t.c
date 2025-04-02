#include "h_switch_table_t.h"

static inline int h_switch_table_hash(value_t value) {
    switch(value.type) {
        case H_VALUE_CHAR:
            return (int)value.character;
        case H_VALUE_NUMBER:
            return (int)value.number;
        case H_VALUE_STRING:
            return (int)value.string->hash;
        default:
            break;
    }
    return 0;
}

h_switch_tables_list_t* h_switch_tables_list_init(size_t capacity) {
    h_switch_tables_list_t* list = (h_switch_tables_list_t*)malloc(sizeof(h_switch_tables_list_t));
    list->switch_table_list = (h_switch_table_t**)malloc(sizeof(h_switch_table_t*) * capacity);
    list->switch_table_size = 0;
    list->switch_table_capacity = capacity;
    return list;
}

h_switch_table_t* h_switch_table_init(size_t capacity) {
    h_switch_table_t* table = (h_switch_table_t*)malloc(sizeof(h_switch_table_t));
    table->entries = (h_switch_table_entry_t*)malloc(sizeof(h_switch_table_entry_t) * capacity);
    table->size = 0;
    table->capacity = capacity;
    return table;
}

void h_switch_table_set(h_switch_table_t* table, value_t value, int index) {
    table->entries[h_switch_table_hash(value) & (table->capacity - 1)] = (h_switch_table_entry_t){.value = value, .index = index};
    ++table->size;
}

inline int h_switch_table_solve(h_switch_table_t* table, value_t value) {
    h_switch_table_entry_t entry = table->entries[h_switch_table_hash(value) & (table->capacity - 1)];
    if(entry.value.type != H_VALUE_UNDEFINED) {
        return entry.index;
    }
    return table->default_index;
}

size_t h_switch_tables_list_add(h_switch_tables_list_t* list, h_switch_table_t* switch_table) {
    if(list->switch_table_size >= list->switch_table_capacity) {
        list->switch_table_capacity <<= 1;
        list->switch_table_list = (h_switch_table_t**)realloc(list->switch_table_list, sizeof(h_switch_table_t*) * list->switch_table_capacity);
    }
    list->switch_table_list[list->switch_table_size] = switch_table;
    return list->switch_table_size++;
}

h_switch_table_t* h_switch_tables_list_get(h_switch_tables_list_t* list, size_t index) {
    return list->switch_table_list[index];
}

int h_switch_tables_list_solve(h_switch_tables_list_t* list, int index, value_t value) {
    return h_switch_table_solve(list->switch_table_list[index], value);
}

void h_switch_table_free(h_switch_table_t* table) {
    free(table->entries);
}

void h_switch_tables_list_free(h_switch_tables_list_t* list) {
    for(size_t i = 0; i < list->switch_table_size; ++i) h_switch_table_free(list->switch_table_list[i]); 
    free(list);
}

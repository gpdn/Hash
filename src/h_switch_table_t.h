#ifndef H_SWITCH_TABLE
#define H_SWITCH_TABLE

#include <stdlib.h>
#include "h_values.h"

typedef struct h_switch_table_entry_t {
    value_t value;
    int index;
} h_switch_table_entry_t;

typedef struct h_switch_table_t {
    h_switch_table_entry_t* entries;
    int default_index;
    size_t size;
    size_t capacity;
    float load_factor;
    value_type_t type;
} h_switch_table_t;

typedef struct h_switch_tables_list_t {
    h_switch_table_t** switch_table_list;
    size_t switch_table_size;
    size_t switch_table_capacity;
} h_switch_tables_list_t;

h_switch_tables_list_t* h_switch_tables_list_init(size_t capacity);
size_t h_switch_tables_list_add(h_switch_tables_list_t* switch_tables_list, h_switch_table_t* switch_table);
h_switch_table_t* h_switch_tables_list_get(h_switch_tables_list_t* switch_tables_list, size_t index);
int h_switch_tables_list_solve(h_switch_tables_list_t* table, int index, value_t value);
void h_switch_tables_list_free(h_switch_tables_list_t* switch_tables_list);

h_switch_table_t* h_switch_table_init(size_t capacity);
void h_switch_table_set(h_switch_table_t* table, value_t value, int index);
int h_switch_table_solve(h_switch_table_t* table, value_t value);
void h_switch_table_free(h_switch_table_t* table);

#endif
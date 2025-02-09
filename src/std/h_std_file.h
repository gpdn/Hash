#ifndef H_STD_FILE
#define H_STD_FILE

#include "h_nfi.h"
#include "../h_file.h"

value_t h_std_file_read_str(struct value_t* parameters, size_t args_count);
value_t h_std_file_write_str(struct value_t* parameters, size_t args_count);
value_t h_std_file_append_str(struct value_t* parameters, size_t args_count);
value_t h_std_file_delete_str(struct value_t* parameters, size_t args_count);
value_t h_std_file_open(struct value_t* parameters, size_t args_count);
value_t h_std_file_close(struct value_t* parameters, size_t args_count);
value_t h_std_file_write(struct value_t* parameters, size_t args_count);
value_t h_std_file_copy(struct value_t* parameters, size_t args_count);
int h_std_file_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif
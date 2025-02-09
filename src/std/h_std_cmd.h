#ifndef H_STD_CMD
#define H_STD_CMD

#include "h_nfi.h"
#include <stdio.h>

typedef enum h_cmd_color_t {
    WHITE,
    RED,
    GRAY,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    RESET,
} h_cmd_color_t;

value_t h_std_cmd_read_char(struct value_t* parameters, size_t args_count);
value_t h_std_cmd_read_line(struct value_t* parameters, size_t args_count);
value_t h_std_cmd_confirm(struct value_t* parameters, size_t args_count);
value_t h_std_cmd_color_set(struct value_t* parameters, size_t args_count);
int h_std_cmd_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif
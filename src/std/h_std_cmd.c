#include "h_std_cmd.h"

#define CMD_BUFFER_SIZE 1024

static const char* cmd_colors_map[] = {
    [WHITE] = "\x1b[0m",
    [RED] = "\x1b[31m",
    [GRAY] = "\x1b[30m",
    [GREEN] = "\x1b[32m",
    [YELLOW] = "\x1b[33m",
    [BLUE] = "\x1b[34m",
    [MAGENTA] = "\x1b[35m",
    [CYAN] = "\x1b[36m",
};

static const char* enum_cmd_color[] = {
    "WHITE",
    "RED",
    "GRAY",
    "GREEN",
    "YELLOW",
    "BLUE",
    "MAGENTA",
    "CYAN"
};

value_t h_std_cmd_read_char(struct value_t* parameters, size_t args_count) {
    printf("%s", H_NFI_STR_TO_CSTRING(parameters[0]));
    char c = (char)fgetchar();
    return CHAR_VALUE(c);
}

value_t h_std_cmd_read_line(struct value_t* parameters, size_t args_count) {
    printf("%s", H_NFI_STR_TO_CSTRING(parameters[0]));
    char input_string[CMD_BUFFER_SIZE];
    fgets(input_string, CMD_BUFFER_SIZE, stdin);
    h_string_t* string = h_string_init(input_string, strlen(input_string) - 1);
    return STR_VALUE(string);
}

value_t h_std_cmd_confirm(struct value_t* parameters, size_t args_count) {
    printf("%s%s", H_NFI_STR_TO_CSTRING(parameters[0]), "[y/n]\n\r");
    char c = (char)fgetchar();
    double result = (c == 'y');
    return NUM_VALUE(result);
}

value_t h_std_cmd_color_set(struct value_t* parameters, size_t args_count) {
    printf("%s", cmd_colors_map[H_NFI_NUM_TO_CINT(parameters[0])]);
    return NULL_VALUE();
}

int h_std_cmd_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    define_native_enum(enums_table, types_table, "CmdColor", enum_cmd_color, sizeof(enum_cmd_color)/sizeof(enum_cmd_color[0]));
    define_native(stack, "cmd_read_char", h_std_cmd_read_char, (value_t[]){(value_t){.type = H_VALUE_STRING}}, 1, (value_t){.type = H_VALUE_CHAR});
    define_native(stack, "cmd_read_line", h_std_cmd_read_line, (value_t[]){(value_t){.type = H_VALUE_STRING}}, 1, (value_t){.type = H_VALUE_STRING});
    define_native(stack, "cmd_confirm", h_std_cmd_confirm, (value_t[]){(value_t){.type = H_VALUE_STRING}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "cmd_color_set", h_std_cmd_color_set, (value_t[]){(value_t){.type = H_VALUE_NUMBER}}, 1, (value_t){.type = H_VALUE_NULL});
    return 1;
}

#undef CMD_BUFFER_SIZE

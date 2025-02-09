#include "h_std_system.h"
#include <windows.h>
#include <direct.h>

value_t h_std_sys_wait(struct value_t* parameters, size_t args_count) {
    int time = H_NFI_NUM_TO_CINT(parameters[0]);
    Sleep(time);
    return NULL_VALUE();
}

value_t h_std_sys_create_directory(struct value_t* parameters, size_t args_count) {
    const char* directory = H_NFI_STR_TO_CSTRING(parameters[0]);
    int result = _mkdir(directory);
    return NUM_VALUE(result == 0 ? 1 : 0);
}

value_t h_std_sys_delete_directory(struct value_t* parameters, size_t args_count) {
    const char* directory = H_NFI_STR_TO_CSTRING(parameters[0]);
    int result = _rmdir(directory);
    return NUM_VALUE(result == 0 ? 1 : 0);
}

value_t h_std_sys_rename(struct value_t* parameters, size_t args_count) {
    const char* from = H_NFI_STR_TO_CSTRING(parameters[0]);
    const char* to = H_NFI_STR_TO_CSTRING(parameters[0]);
    int result = rename(from, to);
    return NUM_VALUE(result == 0 ? 1 : 0);
}

value_t h_std_sys_get_current_directory(struct value_t* parameters, size_t args_count) {
    char buffer[MAX_PATH];
    if(_getcwd(buffer, MAX_PATH) != NULL) {
        return STR_VALUE(h_string_init(buffer, strlen(buffer)));

    }
    return STR_VALUE(h_string_init("", 0));
}

value_t h_std_sys_change_directory(struct value_t* parameters, size_t args_count) {
    const char* directory = H_NFI_STR_TO_CSTRING(parameters[0]);
    int result = _chdir(directory);
    return NUM_VALUE(result == 0 ? 1 : 0);
}

int h_std_sys_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    define_native(stack, "sys_wait", h_std_sys_wait, (value_t[]){H_NFI_VALUE(H_VALUE_NUMBER)}, 1, H_NFI_VALUE(H_VALUE_NULL));
    define_native(stack, "sys_create_directory", h_std_sys_create_directory, (value_t[]){H_NFI_VALUE(H_VALUE_STRING)}, 1, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "sys_delete_directory", h_std_sys_delete_directory, (value_t[]){H_NFI_VALUE(H_VALUE_STRING)}, 1, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "sys_rename", h_std_sys_rename, (value_t[]){H_NFI_VALUE(H_VALUE_STRING), H_NFI_VALUE(H_VALUE_STRING)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "sys_get_current_directory", h_std_sys_get_current_directory, NULL, 0, H_NFI_VALUE(H_VALUE_STRING));
    define_native(stack, "sys_change_directory", h_std_sys_change_directory, (value_t[]){H_NFI_VALUE(H_VALUE_STRING)}, 1, H_NFI_VALUE(H_VALUE_NUMBER));
    return 1;
}
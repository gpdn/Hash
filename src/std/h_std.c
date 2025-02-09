#include "h_std.h"

static h_std_import_fn* imports[] = {
   [H_STD_FLAG_CMD] = h_std_cmd_import,
   [H_STD_FLAG_FILE] = h_std_file_import,
   [H_STD_FLAG_STR] = h_std_string_import,
   [H_STD_FLAG_ARR] = h_std_array_import,
   [H_STD_FLAG_TYPE] = h_std_type_import,
   [H_STD_FLAG_TIME] = h_std_time_import,
   [H_STD_FLAG_TIMER] = h_std_timer_import,
   [H_STD_FLAG_SYSTEM] = h_std_sys_import,
};

h_std_t* h_std_init() {
    h_std_t* std = (h_std_t*)malloc(sizeof(h_std_t));
    std->import_flags = 0;
    return std;
}

int h_std_set_flag(h_std_t* std, h_std_import_flag_t flag) {
    if(std->import_flags & 1 << flag) return 0;
    std->import_flags |= (1 << flag);
    return 1;
}

int h_std_set_flags(h_std_t* std, size_t bitmask) {
    if(std->import_flags & bitmask) return 0;
    std->import_flags |= bitmask;
    return 1;
}

void h_std_print_flags(h_std_t* std) {
    for(size_t i = 0; i < sizeof(size_t); ++i) {
        if(std->import_flags & (1 << i)) {
            DEBUG_LOG("%s\n", h_std_resolve_import_flag(i));
        }   
    }
}

int h_std_resolve_imports(h_std_t* std, h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    for(size_t i = 0; i < sizeof(size_t); ++i) {
        if(std->import_flags & (1 << i)) {
            if(imports[i](stack, enums_table, types_table) == 0) return 0;
            #if DEBUG_STD_FLAGS
                DEBUG_LOG("Imported %s\n", h_std_resolve_import_flag(i));
            #endif
        }
    }
    return 1;
}

void h_std_free(h_std_t* std) {
    free(std);
}
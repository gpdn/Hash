

#include "h_std_time.h"

value_t h_std_time_get_current(struct value_t* parameters, size_t args_count) {
    time_t current = time(NULL);

    return NULL_VALUE();
}

int h_std_time_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    //define_native_data(types_table, "Time", (h_native_data_entry_t[]){
    //    (h_native_data_entry_t){.name = "timestamp", .value = (value_t){.type = H_VALUE_NUMBER}}
    //}, 1);
    return 1;
}
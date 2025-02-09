#include "h_nfi.h"

void define_native(h_locals_stack_t* stack, const char* name, h_native_fn_t function, value_t* params, size_t params_count, value_t return_type) {
    h_string_t* string = h_string_init_hash(name, strlen(name));
    for(size_t i = 0; i < params_count; ++i) {
        printf("%s\n", resolve_value_type(params[i].type));
    }
    h_native_t* native = h_native_init_set(string, function, params, params_count, return_type);    
    h_locals_stack_push(stack, string, VALUE_NATIVE(native), 0);
}

void define_native_enum(h_ht_enums_t* enums_table, h_ht_types_t* types_table, const char* name, const char** entries, size_t entries_count) {
    h_string_t* string = h_string_init_hash(name, strlen(name));
    ht_enum_t* values = h_ht_enums_set(enums_table, string);
    for(size_t i = 0; i < entries_count; ++i) {
        h_string_t* entry = h_string_init_hash(entries[i], strlen(entries[i]));
        h_ht_enum_value_set(values, entry);
    }
    h_ht_types_set(types_table, values->name, NUM_VALUE(0), H_TYPE_INFO_ENUM);
}

void define_native_data(h_ht_types_t* types_table, const char* name, h_native_data_entry_t* entries, size_t entries_count) {
    h_string_t* string = h_string_init_hash(name, strlen(name));
    h_struct_t* data = h_struct_init(string);
    for(size_t i = 0; i < entries_count; ++i) {
        h_string_t* name = h_string_init_hash(entries[i].name, strlen(entries[i].name));
        if(entries[i].value.type == H_VALUE_TYPE) {
            h_string_t* type_name = h_string_init_hash((const char*)entries[i].value.string, strlen((const char*)entries[i].value.string));
            h_struct_field_add(data, name, (value_t){.type = H_VALUE_TYPE, .string = type_name});
        }
        h_struct_field_add(data, name, entries[i].value);
    }
    h_ht_types_set(types_table, string, VALUE_DATA(data), H_TYPE_INFO_DATA);
}
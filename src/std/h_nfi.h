#ifndef H_NFI 
#define H_NFI

#include "../h_values.h"
#include "../h_locals_stack.h"
#include "../h_ht_enums.h"
#include "../h_ht_types.h"

#define H_NFI_STR_TO_CSTRING(value) value.string->string
#define H_NFI_STR_TO_STRING(value) value.string
#define H_NFI_STR_LEN(value) value.string->length
#define H_NFI_ARR_LEN(value) value.array->size
#define H_NFI_NUM_TO_CINT(value) (int)value.number
#define H_NFI_NUM_TO_CDOUBLE(value) value.number
#define H_NFI_NUM_TO_CSIZE(value) (size_t)value.number
#define H_NFI_TYPE_TO_CTYPE(value) value.data_type->other
#define H_NFI_TYPE_TO_TYPE(value) value.data_type
#define H_NFI_TYPE_SET_DATA(data_type, index, value) data_type->data[index] = value
#define H_NFI_TYPE_GET_DATA(data_type, index) data_type->data[index]
#define H_NFI_ARR(value) value.array

#define H_NFI_VALUE(t) (value_t){.type = t}
#define H_NFI_VALUE_TYPE(t, name) (value_t){.type = t, .string = name}

typedef struct h_native_data_entry_t {
    const char* name;
    value_t value;
} h_native_data_entry_t;

void define_native(h_locals_stack_t* stack, const char* name, h_native_fn_t function, value_t* params, size_t params_count, value_t return_type);
void define_native_enum(h_ht_enums_t* enums_table, h_ht_types_t* types_table, const char* name, const char** entries, size_t entries_count);
void define_native_data(h_ht_types_t* types_table, const char* name, h_native_data_entry_t* entries, size_t entries_count);

#endif
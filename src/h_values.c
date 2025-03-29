#include "h_values.h"

void print_value(value_t* value) {
    switch(value->type) {
        case H_VALUE_NUMBER:
            printf("%f\n", value->number);
            break;
        case H_VALUE_STRING:
            printf("%s\n", value->string->string);
            break;
        case H_VALUE_CHAR:
            printf("%c\n", value->character);
            break;
        case H_VALUE_NULL:
            printf("Null\n");
            break;
        case H_VALUE_ARRAY:
            printf("[");
            h_array_print_no_newline(value->array);
            printf("]\n");
            break;
        case H_VALUE_FUNCTION:
            h_function_print(value->function);
            break;
        case H_VALUE_DATA:
            h_struct_print(value->data);
            break;
        case H_VALUE_TYPE:
            h_data_print(value->data_type);
            break;
        case H_VALUE_NATIVE:
            h_native_print(value->native_fn);
            break;
        default:
            printf("%s\n", "Unimplemented value");
            break;
    }
}

void print_value_no_newline(value_t* value) {
    switch(value->type) {
        case H_VALUE_NUMBER:
            printf("%f", value->number);
            break;
        case H_VALUE_STRING:
            printf("%s", value->string->string);
            break;
        case H_VALUE_CHAR:
            printf("%c\n", value->character);
            break;
        case H_VALUE_NULL:
            printf("Null");
            break;
        case H_VALUE_ARRAY:
            printf("Array");
            break;
        case H_VALUE_FUNCTION:
            h_function_print_no_newline(value->function);
            break;
        case H_VALUE_DATA:
            printf("Data");
            break;
        case H_VALUE_TYPE:
            h_data_print_no_newline(value->data_type);
            break;
        case H_VALUE_NATIVE:
            h_native_print_no_newline(value->native_fn);
            break;
        default:
            printf("%s\n", "Unimplemented value");
            break;
    }
}

const char* resolve_type(value_t* value) {
    switch(value->type) {
        case H_VALUE_NUMBER:
            return "Num";
        case H_VALUE_STRING:
            return "Str";
        case H_VALUE_CHAR:
            return "Char";
        case H_VALUE_ARRAY:
            return "Arr";
        case H_VALUE_FUNCTION:
            return "Fn";
        case H_VALUE_DATA:
            return "Data";
        case H_VALUE_TYPE:
            return "Custom";
        case H_VALUE_NATIVE:
            return "Native";
        default:
            return "Unk";
    }
}
 
int compare_value(value_t value_1, value_t value_2) {
    switch(value_1.type) {
        case H_VALUE_NUMBER:
            return value_1.number == value_2.number;
        case H_VALUE_STRING:
            return value_1.string->hash == value_2.string->hash && 
                   value_1.string->length == value_2.string->length &&
                   memcmp(value_1.string, value_2.string, value_1.string->length) == 0;
        case H_VALUE_CHAR:
            return value_1.character == value_2.character;
        case H_VALUE_TYPE:
            if(value_1.data_type->size != value_2.data_type->size) return 0;
            return memcmp(value_1.data_type->data, value_2.data_type->data, value_1.data_type->size * sizeof(value_t*)) == 0;
        case H_VALUE_ARRAY:
            if(value_1.array->size != value_2.array->size) return 0;
            return memcmp(value_1.array->data, value_2.array->data, value_1.array->size * sizeof(value_t*)) == 0;
        default:
            printf("Add value comparison in file %s, at line %d\n", __FILE__, __LINE__);
            return 0;
    }
}

value_t copy_value(value_t value) {
    switch(value.type) {
        case H_VALUE_NUMBER:
        case H_VALUE_CHAR:
        case H_VALUE_NULL:
        case H_VALUE_STRING:
            return value;
        case H_VALUE_ARRAY:
            /* h_array_t* new_array = (h_array_t*)malloc(sizeof(h_array_t));
            memcpy(new_array, value.array, sizeof(h_array_t));
            return (value_t){.type = H_VALUE_ARRAY, .array = new_array}; */
            return value;
        case H_VALUE_TYPE:
            h_data_t* new_data = (h_data_t*)malloc(sizeof(h_data_t));
            value_t* new_data_value = (value_t*)malloc(sizeof(value_t) * value.data_type->capacity);
            memcpy(new_data, value.data_type, sizeof(h_data_t));
            memcpy(new_data_value, new_data->data, sizeof(value_t) * new_data->capacity);
            new_data->data = new_data_value;
            //copy "other" field
            return (value_t){.type = H_VALUE_TYPE, .data_type = new_data};
        default:
            return value;
    }
}

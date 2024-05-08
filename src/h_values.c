#include "h_values.h"

void print_value(value_t* value) {
    switch(value->type) {
        case H_VALUE_NUMBER:
            printf("%f\n", value->number);
            break;
        case H_VALUE_STRING:
            printf("%s\n", value->string->string);
            break;
        case H_VALUE_NULL:
            printf("%s\n", value->string->string);
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
        case H_VALUE_NULL:
            printf("%s", value->string->string);
            break;
    }
}

const char* resolve_type(value_t* value) {
    switch(value->type) {
        case H_VALUE_NUMBER:
            return "Num";
        case H_VALUE_STRING:
            return "Str";
        default:
            return "Unk";
    }
}
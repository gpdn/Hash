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
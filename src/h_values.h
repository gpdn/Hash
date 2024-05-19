#ifndef H_VALUES_H
#define H_VALUES_H

struct h_array_t;

#include <stdio.h>
#include <stdlib.h>
#include "h_value_types.h"
#include "h_string_t.h"
#include "h_array_t.h"
#include "h_function_t.h"

//typedef double value_t;

typedef struct value_t {
    value_type_t type;
    union {
        double number;
        char character;
        h_string_t* string;
        struct h_array_t* array;
        h_function_t* function;
    };
} value_t;

#define NUM_VALUE(value) ((value_t){H_VALUE_NUMBER, {.number = value}})
#define STR_VALUE(value) ((value_t){H_VALUE_STRING, {.string = value}})
#define CHAR_VALUE(value) ((value_t){H_VALUE_CHAR, {.char = value}})
#define NULL_VALUE(value) ((value_t){H_VALUE_NULL, {.number = 0}})
#define UNDEFINED_VALUE(value) ((value_t){H_VALUE_UNDEFINED, {.number = 0}})
#define VALUE_ARRAY(value) ((value_t){H_VALUE_ARRAY, {.array = value}})
#define VALUE_FUNCTION(value) ((value_t){H_VALUE_FUNCTION, {.function = value}})

#define IS_NUM(value) ((value).type == H_VALUE_NUMBER);
#define IS_STR(value) ((value).type == H_VALUE_STRING);
#define IS_CHAR(value) ((value).type == H_VALUE_CHAR);
#define IS_NULL(value) ((value).type == H_VALUE_NULL);
#define IS_UNDEFINED(value) ((value).type == H_VALUE_UNDEFINED);
#define IS_ARRAY(value) ((value).type == H_VALUE_ARRAY);
#define IS_FUNCTION(value) ((value).type == H_VALUE_FUNCTION);

void print_value(value_t* value);
void print_value_no_newline(value_t* value);
const char* resolve_type(value_t* value);

#endif
#include "h_std_string.h"

static inline const char* std_string_reverse(const char* string, size_t length) {
    char* reversed_string = (char*)malloc(sizeof(char) * length + 1);
    for(size_t i = 0; i < length; ++i) {
        reversed_string[i] = string[length - i];
    }
    return reversed_string;
}

value_t h_std_str_compare(struct value_t* parameters, size_t args_count) {
    h_string_t* string_1 = H_NFI_STR_TO_STRING(parameters[0]);
    h_string_t* string_2 = H_NFI_STR_TO_STRING(parameters[1]);
    return NUM_VALUE(
            (
                string_1->length == string_2->length 
                && memcmp(string_1->string, string_2->string, string_1->length) == 0
            ) != 0 ? 1 : 0
        );
}

value_t h_std_str_concat(struct value_t* parameters, size_t args_count) {
    h_string_t* string_1 = H_NFI_STR_TO_STRING(parameters[0]);
    h_string_t* string_2 = H_NFI_STR_TO_STRING(parameters[1]);
    h_string_t* new_string = (h_string_t*)malloc(sizeof(h_string_t));
    new_string->length = string_1->length + string_2->length;
    new_string->string = (char*)malloc(sizeof(char) * new_string->length);
    memcpy(new_string->string, string_1->string, string_1->length);
    memcpy(new_string->string + string_1->length, string_2->string, string_2->length);
    new_string->string[new_string->length] = '\0';
    return STR_VALUE(new_string);
}

value_t h_std_str_substr(struct value_t* parameters, size_t args_count) {
    h_string_t* string = H_NFI_STR_TO_STRING(parameters[0]);
    size_t start = H_NFI_NUM_TO_CSIZE(parameters[1]);
    size_t offset = H_NFI_NUM_TO_CSIZE(parameters[2]);
    if(start + offset > string->length) return STR_VALUE(string);
    h_string_t* new_string = (h_string_t*)malloc(sizeof(h_string_t));
    new_string->length = offset - start + 1;
    new_string->string = (char*)malloc(sizeof(char) * new_string->length);
    new_string->string = memcpy(new_string->string, string->string + start, offset);
    return STR_VALUE(new_string);
}

value_t h_std_str_find(struct value_t* parameters, size_t args_count) {
    const char* string = H_NFI_STR_TO_CSTRING(parameters[0]);
    const char* string_to_find = H_NFI_STR_TO_CSTRING(parameters[1]);
    size_t string_to_find_length = H_NFI_STR_LEN(parameters[1]);
    if(string_to_find_length == 0) return NUM_VALUE(-1);
    char* position = strstr(string, string_to_find);
    if(position) return NUM_VALUE(position - string);
    return NUM_VALUE(-1);
}

value_t h_std_str_reverse(struct value_t* parameters, size_t args_count) {
    const char* string = H_NFI_STR_TO_CSTRING(parameters[0]);
    size_t string_to_find_length = H_NFI_STR_LEN(parameters[0]);
    if(string_to_find_length == 0) return parameters[0];
    const char* reversed_string = std_string_reverse(string, string_to_find_length);
    h_string_t* new_string = h_string_init_hash_set(reversed_string, string_to_find_length);
    return STR_VALUE(new_string);
}

value_t h_std_str_find_all(struct value_t* parameters, size_t args_count) {
    const char* string = H_NFI_STR_TO_CSTRING(parameters[0]);
    const char* string_to_find = H_NFI_STR_TO_CSTRING(parameters[1]);
    size_t string_to_find_length = H_NFI_STR_LEN(parameters[1]);
    h_array_t* array = h_array_init(H_VALUE_NUMBER, 5);
    if(string_to_find_length == 0) return VALUE_ARRAY(array);
    char* position = strstr(string, string_to_find);
    if(position == NULL) return VALUE_ARRAY(array);
    do {
        h_array_push(array, NUM_VALUE(position - string));
        position = strstr(string, string_to_find);
    } while(position);
    return VALUE_ARRAY(array);
}

value_t h_std_str_len(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(H_NFI_STR_LEN(parameters[0]));
}

value_t h_std_str_replace(struct value_t* parameters, size_t args_count) {
    const char* string = H_NFI_STR_TO_CSTRING(parameters[0]);
    const char* replace = H_NFI_STR_TO_CSTRING(parameters[1]);
    const char* with = H_NFI_STR_TO_CSTRING(parameters[2]);
    size_t string_length = H_NFI_STR_LEN(parameters[0]);
    size_t replace_length = H_NFI_STR_LEN(parameters[1]);
    size_t with_length = H_NFI_STR_LEN(parameters[2]);

    if(string_length == 0 || replace_length == 0) return NUM_VALUE(0);

    size_t replaced_string_size = string_length + with_length - replace_length;

    char* replaced_string = (char*)malloc(sizeof(char) * replaced_string_size);

    printf("Replace %s with %s\n", replace, with);

    for(size_t i = 0; i < replaced_string_size; ++i) {
        printf("%lld: %c - %c\n", i, string[i], replace[0]);
        if(string[i] == replace[0]) {
            printf("in loop\n");
            printf("replace length: %lld\n", replace_length);
            size_t j = 0;
            while(++j < replace_length && string[i + j] == replace[j]);
            printf("%lld: %c - %c\n", j, string[i + j], replace[j - 1]);
            if(j == replace_length) {
                printf("Write\n");
                memcpy(replaced_string + i, with, replace_length);
                break;
            }
        }
        replaced_string[i] = string[i];
    }

    replaced_string[replaced_string_size] = '\0';

    printf("%s - %s\n", string, replaced_string);

    h_string_t* return_string = h_string_init_hash_set(replaced_string, replaced_string_size);

    return STR_VALUE(return_string);

}

/* value_t h_std_str_find_last(struct value_t* parameters, size_t args_count) {
    const char* string = H_NFI_STR_TO_CSTRING(parameters[0]);
    const char* string_to_find = H_NFI_STR_TO_CSTRING(parameters[1]);
    int string_to_find_length = H_NFI_STR_LEN(parameters[1]);
    if(string_to_find_length == 0) return NUM_VALUE(-1);
    const char* reversed_string = std_string_reverse(string, string_to_find_length);
    char* position = strstr(string, string_to_find);
    if(position) return NUM_VALUE(position - string);
    return NUM_VALUE(-1);
} */

int h_std_string_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    define_native(stack, "str_compare", h_std_str_compare, (value_t[]){H_NFI_VALUE(H_VALUE_STRING), H_NFI_VALUE(H_VALUE_STRING)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "str_concat", h_std_str_concat, (value_t[]){H_NFI_VALUE(H_VALUE_STRING), H_NFI_VALUE(H_VALUE_STRING)}, 2, H_NFI_VALUE(H_VALUE_STRING));
    define_native(stack, "str_substr", h_std_str_substr, (value_t[]){H_NFI_VALUE(H_VALUE_STRING), H_NFI_VALUE(H_VALUE_NUMBER), H_NFI_VALUE(H_VALUE_NUMBER)}, 3, H_NFI_VALUE(H_VALUE_STRING));
    define_native(stack, "str_find", h_std_str_find, (value_t[]){H_NFI_VALUE(H_VALUE_STRING), H_NFI_VALUE(H_VALUE_STRING)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "str_reverse", h_std_str_reverse, (value_t[]){H_NFI_VALUE(H_VALUE_STRING)}, 1, H_NFI_VALUE(H_VALUE_STRING));
    define_native(stack, "str_find_all", h_std_str_find_all, (value_t[]){H_NFI_VALUE(H_VALUE_STRING), H_NFI_VALUE(H_VALUE_STRING)}, 2, H_NFI_VALUE(H_VALUE_ARRAY));
    define_native(stack, "str_len", h_std_str_len, (value_t[]){H_NFI_VALUE(H_VALUE_STRING)}, 1, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "str_replace", h_std_str_replace, (value_t[]){H_NFI_VALUE(H_VALUE_STRING), H_NFI_VALUE(H_VALUE_STRING), H_NFI_VALUE(H_VALUE_STRING)}, 3, H_NFI_VALUE(H_VALUE_STRING));
    return 1;
}
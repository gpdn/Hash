#include "h_std_file.h"

value_t h_std_file_read_str(struct value_t* parameters, size_t args_count) {
    const char* filename = H_NFI_STR_TO_CSTRING(parameters[0]);
    FILE* file = fopen(filename, "rb");
    if(file == NULL) {
        return STR_VALUE(h_string_init_hash("", 0));
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(sizeof(char) * (file_size + 1));
    size_t last_byte_read = fread(buffer, sizeof(char), file_size, file);
    if(last_byte_read < file_size) {
        free(buffer);
        return STR_VALUE(h_string_init_hash("", 0));
    }
    buffer[last_byte_read] = '\0';
    fclose(file);
    h_string_t* string = h_string_init_hash(buffer, file_size + 1);
    return STR_VALUE(string);
}

value_t h_std_file_write_str(struct value_t* parameters, size_t args_count) {
    const char* filename = H_NFI_STR_TO_CSTRING(parameters[0]);
    const char* content = H_NFI_STR_TO_CSTRING(parameters[1]);
    FILE* file = fopen(filename, "wb");
    if(file == NULL) {
        return NUM_VALUE(0);
    }
    int result = fputs(content, file);
    return NUM_VALUE(result > 0 ? 1 : 0);
}

value_t h_std_file_append_str(struct value_t* parameters, size_t args_count) {
    const char* filename = H_NFI_STR_TO_CSTRING(parameters[0]);
    const char* content = H_NFI_STR_TO_CSTRING(parameters[1]);
    FILE* file = fopen(filename, "ab");
    if(file == NULL) {
        return NUM_VALUE(0);
    }
    int result = fputs(content, file);
    return NUM_VALUE(result > 0 ? 1 : 0);
}

value_t h_std_file_delete_str(struct value_t* parameters, size_t args_count) {
    const char* filename = H_NFI_STR_TO_CSTRING(parameters[0]);
    int result = remove(filename);
    return NUM_VALUE(result == 0 ? 1 : 0);
}

value_t h_std_file_open(struct value_t* parameters, size_t args_count) {
    const char* filename = H_NFI_STR_TO_CSTRING(parameters[0]);
    FILE* file = fopen(filename, "wb+");
    h_data_t* data = h_data_init(1);
    h_string_t* type_name = h_string_init_hash("File", strlen("File"));
    data->type_name = type_name;
    data->other = file;
    data->size = 1;
    data->data[0] = (value_t){.type = H_VALUE_STRING, .string = h_string_init(filename, strlen(filename))};
    return VALUE_TYPE(data);
}

value_t h_std_file_close(struct value_t* parameters, size_t args_count) {
    FILE* file = H_NFI_TYPE_TO_CTYPE((FILE*)parameters[0]);
    if(!file) return NUM_VALUE(0);
    int result = fclose(file);
    return NUM_VALUE(result == 0 ? 1 : 0);
}

value_t h_std_file_write(struct value_t* parameters, size_t args_count) {
    FILE* file = H_NFI_TYPE_TO_CTYPE((FILE*)parameters[0]);
    const char* content = H_NFI_STR_TO_CSTRING(parameters[1]);
    int result = fprintf(file, "%s", content);
    return NUM_VALUE(result > 0 ? 1 : 0);
}

value_t h_std_file_copy(struct value_t* parameters, size_t args_count) {
    FILE* file = H_NFI_TYPE_TO_CTYPE((FILE*)parameters[0]);
    const char* filename = H_NFI_STR_TO_CSTRING(parameters[1]);
    
    FILE* file2 = fopen(filename, "wb");
    
    if(file2 == NULL) {
        return NUM_VALUE(0);
    }

    long cursor = ftell(file);

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(sizeof(char) * (file_size + 1));
    size_t last_byte_read = fread(buffer, sizeof(char), file_size, file);
    if(last_byte_read < file_size) {
        free(buffer);
        return NUM_VALUE(0);
    }
    buffer[last_byte_read] = '\0';

    int result = fprintf(file, "%s", buffer);
    fclose(file2);

    fseek(file, cursor, SEEK_CUR);

    return NUM_VALUE(result > 0 ? 1 : 0);
}

int h_std_file_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    define_native_data(types_table, "File", (h_native_data_entry_t[]){
        (h_native_data_entry_t){.name = "path", .value = (value_t){.type = H_VALUE_STRING}}
    }, 1);
    define_native(stack, "file_read_str", h_std_file_read_str, (value_t[]){(value_t){.type = H_VALUE_STRING}}, 1, (value_t){.type = H_VALUE_STRING});
    define_native(stack, "file_write_str", h_std_file_write_str, (value_t[]){(value_t){.type = H_VALUE_STRING}, (value_t){.type = H_VALUE_STRING}}, 2, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "file_append_str", h_std_file_append_str, (value_t[]){(value_t){.type = H_VALUE_STRING}, (value_t){.type = H_VALUE_STRING}}, 2, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "file_delete_str", h_std_file_delete_str, (value_t[]){(value_t){.type = H_VALUE_STRING}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "file_open", h_std_file_open, (value_t[]){(value_t){.type = H_VALUE_STRING}}, 1, (value_t){.type = H_VALUE_TYPE, .string = h_string_init_hash("File", strlen("File"))});
    define_native(stack, "file_close", h_std_file_close, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = h_string_init_hash("File", strlen("File"))}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "file_write", h_std_file_write, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = h_string_init_hash("File", strlen("File"))}, (value_t){.type = H_VALUE_STRING}}, 2, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "file_copy", h_std_file_copy, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = h_string_init_hash("File", strlen("File"))}, (value_t){.type = H_VALUE_STRING}}, 2, (value_t){.type = H_VALUE_NUMBER});
    return 1;
}
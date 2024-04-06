#include "h_file.h"

const char* read_file(const char* source) {
    FILE* file = fopen(source, "rb");

    if(file == NULL) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(sizeof(char) * (file_size + 1));
    
    size_t last_byte_read = fread(buffer, sizeof(char), file_size, file);

    if(last_byte_read < file_size) {
        free(buffer);
        exit(HASH_FILE_READ_FAILED);
    }

    buffer[last_byte_read] = '\0';

    fclose(file);
    return buffer;
}

size_t write_file(const char* path, const char* content, size_t element_size, size_t elements_count) {
    FILE* file = fopen(path, "wb");

    if(file == NULL) {
        DEBUG_LOG("Failed to open file(Path: %s).\n", path);
        return 0;
    }  

    size_t count = fwrite(content, element_size, elements_count, file);

    DEBUG_LOG("COUNT: %lld\n", count);

    if(count != elements_count) {
        DEBUG_LOG("Failed to write to file(Path: %s). Content and number of elements not matching\n", path);
        return 0;
    }

    fclose(file);

    return count;
}

size_t append_file(FILE* file, const char* content, size_t element_size, size_t elements_count) {
    return fwrite(content, element_size, elements_count, file);
}
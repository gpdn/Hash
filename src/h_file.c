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
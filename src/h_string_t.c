#include "h_string_t.h"

static size_t h_ht_hash(const char* string, size_t length);

static size_t h_ht_hash(const char* string, size_t length) {
    uint32_t hash = 2166136261u;
    for(const char* temp = string; temp != string + length; ++temp) {
        hash ^= (uint8_t)*temp;
        hash *= 16777619;
    }
    return hash;
}

h_string_t* h_string_init(const char* string, size_t length) {
    h_string_t* h_string = (h_string_t*)malloc(sizeof(h_string_t));
    h_string->string = (char*)malloc(sizeof(char) * (length + 1));
    memcpy(h_string->string, string, length);
    h_string->string[length] = '\0';
    h_string->length = length;
    h_string->capacity = length;
    h_string->hash = 0;
    return h_string;
}

h_string_t* h_string_init_hash(const char* string, size_t length) {
    h_string_t* h_string = (h_string_t*)malloc(sizeof(h_string_t));
    h_string->string = (char*)malloc(sizeof(char) * (length + 1));
    memcpy(h_string->string, string, length);
    h_string->string[length] = '\0';
    h_string->length = length;
    h_string->capacity = length;
    h_string->hash = h_ht_hash(string, length);
    return h_string;
}

h_string_t* h_string_init_hash_set(const char* string, size_t length) {
    h_string_t* h_string = (h_string_t*)malloc(sizeof(h_string_t));
    h_string->string = string;
    h_string->length = length;
    h_string->capacity = length;
    h_string->hash = h_ht_hash(string, length);
    return h_string;
}

uint8_t h_string_compare(h_string_t* string_1, h_string_t* string_2) {
    return string_1->length == string_2->length && memcmp(string_1->string, string_2->string, string_1->length) == 0;
}

h_string_t* h_string_concatenate(h_string_t* string_1, h_string_t* string_2) {
    h_string_t* new_string = (h_string_t*)malloc(sizeof(h_string_t));
    new_string->length = string_1->length + string_2->length;
    new_string->string = (char*)malloc(sizeof(char) * new_string->length);
    memcpy(new_string->string, string_1->string, string_1->length);
    memcpy(new_string->string + string_1->length, string_2->string, string_2->length);
    new_string->string[new_string->length] = '\0';
    return new_string;
}

void h_string_append_cstring(h_string_t* string_1, const char* string_2) {
    size_t length = strlen(string_2);
    if(string_1->length + length > string_1->capacity) {
        string_1->capacity = string_1->capacity * 2;
        string_1->string = (char*)malloc(sizeof(char) * string_1->capacity);
    }
    memcpy(string_1->string + string_1->length, string_2, length);
    string_1->string[string_1->length] = '\0';
}

char h_string_get(h_string_t* string, size_t index) {
    return string->string[index];
}

void h_string_set(h_string_t* string, char c, size_t index) {
    string->string[index] = c;
}

h_string_t* h_string_substr(h_string_t* string, size_t start, size_t offset) {
    if(start + offset > string->length) return NULL;
    h_string_t* new_string = (h_string_t*)malloc(sizeof(h_string_t));
    new_string->length = offset - start + 1;
    new_string->string = (char*)malloc(sizeof(char) * new_string->length);
    new_string->string = memcpy(new_string->string, string->string + start, offset);
    return new_string;
}

void h_string_free(h_string_t* h_string) {
    free(h_string->string);
    free(h_string);
}
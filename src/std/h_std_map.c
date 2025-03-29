#include "h_std_map.h"

#define H_STD_DEFAULT_MAP_CAPACITY 64
#define H_STD_DEFAULT_MAP_LOAD_FACTOR 0.75

typedef struct h_map_entry_t {
    h_string_t* key;
    value_t value;
    struct h_map_entry_t* collision;
} h_map_entry_t;

typedef struct h_map_t {
    h_map_entry_t* data;
    size_t size;
    size_t capacity;
} h_map_t;

static inline h_data_t* h_map_data_make();
static void h_map_init_data_placeholder();
static size_t h_map_hash(h_string_t* string);

static h_string_t* map_string_placeholder;
static value_t map_data_placeholder;

static void h_map_init_data_placeholder() {
    map_string_placeholder = h_string_init_hash("Map", strlen("Map"));
    map_data_placeholder = (value_t){.type = H_VALUE_TYPE, .data_type = h_map_data_make(NULL)};
}

static inline h_data_t* h_map_data_make(h_map_t* map_data) {
    h_data_t* data = h_data_init(0);
    data->type_name = map_string_placeholder;
    data->other = map_data;
    data->size = 0;
    return data;
}

static size_t h_map_hash(h_string_t* string) {
    uint32_t hash = 2166136261u;
    for(const char* temp = string->string; temp != string->string + string->length; ++temp) {
        hash ^= (uint8_t)*temp;
        hash *= 16777619;
    }
    return hash;
}

value_t h_std_map_init(struct value_t* parameters, size_t args_count) {
    h_map_t* map = (h_map_t*)malloc(sizeof(h_map_t));
    map->data = (h_map_entry_t*)calloc(H_STD_DEFAULT_MAP_CAPACITY, sizeof(h_map_entry_t));
    map->size = 0;
    map->capacity = H_STD_DEFAULT_MAP_CAPACITY;
    return VALUE_TYPE(h_map_data_make(map));
}

value_t h_std_map_set(struct value_t* parameters, size_t args_count) {
    h_map_t* map = (h_map_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    h_string_t* key = H_NFI_STR_TO_STRING(parameters[1]);
    value_t value = parameters[2];
    size_t index = h_map_hash(key) & (map->capacity - 1);
    h_map_entry_t* entry = map->data + index;
    entry->key = key;
    entry->value = value;
    ++map->size;
    return NUM_VALUE(1);
}

value_t h_std_map_get(struct value_t* parameters, size_t args_count) {
    h_map_t* map = (h_map_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    h_string_t* key = H_NFI_STR_TO_STRING(parameters[1]);
    size_t index = h_map_hash(key) & (map->capacity - 1);
    h_map_entry_t* entry = &map->data[index];
    return entry->key != NULL ? entry->value : NULL_VALUE();
}

value_t h_std_map_unset(struct value_t* parameters, size_t args_count) {
    h_map_t* map = (h_map_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    h_string_t* key = H_NFI_STR_TO_STRING(parameters[1]);
    size_t index = h_map_hash(key) & (map->capacity - 1);
    if((map->data[index].key) == NULL) return NUM_VALUE(0);
    memset(map->data + index, 0, sizeof(h_map_entry_t));
    --map->size;
    return NUM_VALUE(1);
}

value_t h_std_map_clear(struct value_t* parameters, size_t args_count) {
    h_map_t* map = (h_map_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    map->size = 0;
    memset(map->data, 0, sizeof(h_map_entry_t) * map->capacity);
    return NULL_VALUE();
}

value_t h_std_map_check(struct value_t* parameters, size_t args_count) {
    h_map_t* map = (h_map_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    h_string_t* key = H_NFI_STR_TO_STRING(parameters[1]);
    size_t index = h_map_hash(key) & (map->capacity - 1);
    h_map_entry_t* entry = &map->data[index];
    return NUM_VALUE(entry->key != NULL);
}

value_t h_std_map_size(struct value_t* parameters, size_t args_count) {
    h_map_t* map = (h_map_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return NUM_VALUE(map->size);
}

value_t h_std_map_print(struct value_t* parameters, size_t args_count) {
    h_map_t* map = (h_map_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    for(size_t i = 0; i < map->capacity; ++i) {
        if(map->data[i].key == NULL) continue;
        printf("%.*s - ", (int)map->data[i].key->length, map->data[i].key->string);
        print_value_no_newline(&map->data[i].value);
    }
    printf("\n");
    return NULL_VALUE();
}

int h_std_map_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    h_map_init_data_placeholder();
    define_native_data(types_table, "Map", NULL, 0);
    define_native(stack, "map_init", h_std_map_init, NULL, 0, map_data_placeholder);
    define_native(stack, "map_set", h_std_map_set, (value_t[]){map_data_placeholder, H_NFI_VALUE(H_VALUE_STRING), H_NFI_VALUE(H_VALUE_GENERIC)}, 3, H_NFI_VALUE(H_VALUE_NULL));
    define_native(stack, "map_get", h_std_map_get, (value_t[]){map_data_placeholder, H_NFI_VALUE(H_VALUE_STRING)}, 2, H_NFI_VALUE(H_VALUE_GENERIC));
    define_native(stack, "map_unset", h_std_map_unset, (value_t[]){map_data_placeholder, H_NFI_VALUE(H_VALUE_STRING)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "map_clear", h_std_map_clear, (value_t[]){map_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_NULL));
    define_native(stack, "map_check", h_std_map_check, (value_t[]){map_data_placeholder, H_NFI_VALUE(H_VALUE_STRING)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "map_size", h_std_map_size, (value_t[]){map_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "map_print", h_std_map_print, (value_t[]){map_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_NULL));
    return 1;
}

#undef H_STD_DEFAULT_MAP_CAPACITY
#undef H_STD_DEFAULT_MAP_LOAD_FACTOR
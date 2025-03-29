
#include "h_std_dll.h"

typedef struct h_dll_node_t {
    value_t value;
    struct h_dll_node_t* prev;
    struct h_dll_node_t* next;
} h_dll_node_t;

typedef struct h_dll_t {
    h_dll_node_t* head;
    h_dll_node_t* tail;
    size_t size;
} h_dll_t;

static inline h_data_t* h_dll_data_make();
static void h_dll_init_data_placeholder();
static h_dll_node_t* h_dll_node_make(value_t value, h_dll_node_t* prev, h_dll_node_t* next);

static h_string_t* dll_string_placeholder;
static value_t dll_data_placeholder;

static void h_dll_init_data_placeholder() {
    dll_string_placeholder = h_string_init_hash("DLList", strlen("DLList"));
    dll_data_placeholder = (value_t){.type = H_VALUE_TYPE, .data_type = h_dll_data_make(NULL)};
}

static inline h_data_t* h_dll_data_make(h_dll_t* dll_data) {
    h_data_t* data = h_data_init(0);
    data->type_name = dll_string_placeholder;
    data->other = dll_data;
    data->size = 0;
    return data;
}

static h_dll_node_t* h_dll_node_make(value_t value, h_dll_node_t* prev, h_dll_node_t* next) {
    h_dll_node_t* node = (h_dll_node_t*)malloc(sizeof(h_dll_node_t));
    node->value = value;
    node->prev = prev;
    if(prev) prev->next = node;
    node->next = next;
    if(next) next->prev = node;
    return node; 
}

value_t h_std_dll_init(struct value_t* parameters, size_t args_count) {
    h_dll_t* dll_data = (h_dll_t*)malloc(sizeof(h_dll_t));
    dll_data->head = NULL;
    dll_data->tail = NULL;
    dll_data->size = 0;
    return VALUE_TYPE(h_dll_data_make(dll_data));
}

value_t h_std_dll_append_head(struct value_t* parameters, size_t args_count) {
    h_dll_t* data = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_dll_node_t* node = h_dll_node_make(value, NULL, data->head);
    data->head = node;
    if(!data->tail) data->tail = data->head;
    ++data->size;
    return NUM_VALUE(1);
}

value_t h_std_dll_append_tail(struct value_t* parameters, size_t args_count) {
    h_dll_t* data = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_dll_node_t* node = h_dll_node_make(value, data->tail, NULL);
    data->tail = node;
    if(!data->head) data->head = data->tail;
    return NUM_VALUE(++data->size);
}

value_t h_std_dll_remove_first(struct value_t* parameters, size_t args_count) {
    h_dll_t* data = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_dll_node_t* it = data->head;
    int index = 0;
    for(; it != NULL; (it = it->next) && ++index) {
        if(compare_value(it->value, value)) {
            if(it->prev) {
                it->prev->next = it->next;
                if(it->next) it->next->prev = it->prev;
                --data->size;
                return NUM_VALUE(index);
            }
            data->head = it->next;
            --data->size;
            return NUM_VALUE(index);
        }
    }
    return NUM_VALUE(-1);
}

value_t h_std_dll_remove_last(struct value_t* parameters, size_t args_count) {
    h_dll_t* data = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_dll_node_t* it = data->tail;
    int index = (int)data->size;
    for(; it != NULL; (it = it->prev) && --index) {
        if(compare_value(it->value, value)) {
            if(it->next) {
                it->next->prev = it->prev;
                if(it->prev) it->prev->next = it->next;
                --data->size;
                return NUM_VALUE(index);
            }
            data->tail = it->next;
            --data->size;
            return NUM_VALUE(index);
        }
    }
    return NUM_VALUE(-1);
}

value_t h_std_dll_remove_all(struct value_t* parameters, size_t args_count) {
    h_dll_t* data = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_dll_node_t* it = data->head;
    h_array_t* indices = h_array_init(parameters[0].type, 5);
    int index = 0;
    for(; it != NULL; (it = it->next) && ++index) {
        if(compare_value(it->value, value)) {
            h_array_push(indices, NUM_VALUE(index));
            --data->size;
            if(it->prev) {
                it->prev->next = it->next;
                if(it->next) it->next->prev = it->prev;
                continue;
            }
            data->head = it->next;
        }
    }
    return NUM_VALUE(-1);
}

value_t h_std_dll_replace_first(struct value_t* parameters, size_t args_count) {
    h_dll_t* data = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    value_t value_2 = parameters[2];
    h_dll_node_t* it = data->head;
    int index = 0;
    for(; it != NULL; (it = it->next) && ++index) {
        if(compare_value(it->value, value)) {
            it->value = value_2;
            return NUM_VALUE(index);
        }
    }
    return NUM_VALUE(-1);
}

value_t h_std_dll_replace_last(struct value_t* parameters, size_t args_count) {
    h_dll_t* data = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    value_t value_2 = parameters[2];
    h_dll_node_t* it = data->tail;
    int index = (int)data->size;
    for(; it != NULL; (it = it->prev) && --index) {
        if(compare_value(it->value, value)) {
            it->value = value_2;
            return NUM_VALUE(index);
        }
    }
    return NUM_VALUE(-1);
}

value_t h_std_dll_replace_all(struct value_t* parameters, size_t args_count) {
    h_dll_t* data = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    value_t value_2 = parameters[2];
    h_dll_node_t* it = data->head;
    int index = 0;
    h_array_t* indices = h_array_init(parameters[0].type, 5);
    for(; it != NULL; (it = it->next) && ++index) {
        if(compare_value(it->value, value)) {
            it->value = value_2;
            h_array_push(indices, NUM_VALUE(index));
        }
    }
    return VALUE_ARRAY(indices);
}

value_t h_std_dll_find_first(struct value_t* parameters, size_t args_count) {
    h_dll_t* data = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_dll_node_t* it = data->head;
    int index = 0;
    for(; it != NULL; (it = it->next) && ++index) {
        if(compare_value(it->value, value)) return NUM_VALUE(index);
    }
    return NUM_VALUE(-1);
}

value_t h_std_dll_find_last(struct value_t* parameters, size_t args_count) {
    h_dll_t* data = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_dll_node_t* it = data->tail;
    int index = (int)data->size - 1;
    for(; it != NULL; (it = it->prev) && --index) {
        if(compare_value(it->value, value)) return NUM_VALUE(index);
    }
    return NUM_VALUE(-1);
}

value_t h_std_dll_find_all(struct value_t* parameters, size_t args_count) {
    h_dll_t* data = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_dll_node_t* it = data->head;
    int index = 0;
    h_array_t* indices = h_array_init(parameters[0].type, 5);
    for(; it != NULL; (it = it->next) && ++index) {
        if(compare_value(it->value, value)) h_array_push(indices, NUM_VALUE(index));
    }
    return VALUE_ARRAY(indices);
}

value_t h_std_dll_combine(struct value_t* parameters, size_t args_count) {
    h_dll_t* list = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    h_dll_t* list_2 = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[1])->other;
    if(!list->tail) return parameters[1];
    if(!list_2->head) return parameters[0];
    list->tail->next = list_2->head;
    list_2->head->prev = list->tail;
    list->tail = list_2->tail;
    return parameters[0];
}

value_t h_std_dll_size(struct value_t* parameters, size_t args_count) {
    h_dll_t* list = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return NUM_VALUE(list->size);
}

value_t h_std_dll_get_first(struct value_t* parameters, size_t args_count) {
    h_dll_t* list = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return list->head ? list->head->value : NULL_VALUE(); 
}

value_t h_std_dll_get_last(struct value_t* parameters, size_t args_count) {
    h_dll_t* list = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return list->tail ? list->tail->value : NULL_VALUE();
}

value_t h_std_dll_get_head(struct value_t* parameters, size_t args_count) {
    h_dll_t* list = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    unsigned int index = H_NFI_NUM_TO_CUINT(parameters[1]) + 1;
    h_dll_node_t* it = list->head;
    if(!list->head) return NULL_VALUE();
    for(; it->next != NULL && (--index > 0); it = it->next);
    return it->value;
}

value_t h_std_dll_get_tail(struct value_t* parameters, size_t args_count) {
    h_dll_t* list = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    unsigned int index = H_NFI_NUM_TO_CUINT(parameters[1]) + 1;
    h_dll_node_t* it = list->tail;
    if(!list->tail) return NULL_VALUE();
    for(; it->prev != NULL && (--index > 0); it = it->prev) {
        DEBUG_LOG("%p\n", it);
    }
    return it->value;
}

value_t h_std_dll_print(struct value_t* parameters, size_t args_count) {
    h_dll_t* data = (h_dll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    for(h_dll_node_t* it = data->head; it != NULL; it = it->next) {
        print_value(&it->value);
    }
    return NULL_VALUE();
}

int h_std_dll_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    h_dll_init_data_placeholder();
    define_native(stack, "dll_init", h_std_dll_init, NULL, 0, dll_data_placeholder);
    define_native_data(types_table, "DLList", NULL, 0);
    define_native(stack, "dll_append_head", h_std_dll_append_head, (value_t[]){dll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NULL));
    define_native(stack, "dll_append_tail", h_std_dll_append_tail, (value_t[]){dll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "dll_remove_first", h_std_dll_remove_first, (value_t[]){dll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "dll_remove_last", h_std_dll_remove_last, (value_t[]){dll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "dll_remove_all", h_std_dll_remove_all, (value_t[]){dll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_ARRAY));
    define_native(stack, "dll_replace_first", h_std_dll_replace_first, (value_t[]){dll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC), H_NFI_VALUE(H_VALUE_GENERIC)}, 3, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "dll_replace_last", h_std_dll_replace_last, (value_t[]){dll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC), H_NFI_VALUE(H_VALUE_GENERIC)}, 3, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "dll_replace_all", h_std_dll_replace_all, (value_t[]){dll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC), H_NFI_VALUE(H_VALUE_GENERIC)}, 3, H_NFI_VALUE(H_VALUE_ARRAY));
    define_native(stack, "dll_find_first", h_std_dll_find_first, (value_t[]){dll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "dll_find_last", h_std_dll_find_last, (value_t[]){dll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "dll_find_all", h_std_dll_find_all, (value_t[]){dll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_ARRAY));
    define_native(stack, "dll_combine", h_std_dll_combine, (value_t[]){dll_data_placeholder, dll_data_placeholder}, 2, dll_data_placeholder);
    define_native(stack, "dll_size", h_std_dll_size, (value_t[]){dll_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "dll_get_first", h_std_dll_get_first, (value_t[]){dll_data_placeholder}, 1, dll_data_placeholder);
    define_native(stack, "dll_get_last", h_std_dll_get_last, (value_t[]){dll_data_placeholder}, 1, dll_data_placeholder);
    define_native(stack, "dll_get_head", h_std_dll_get_head, (value_t[]){dll_data_placeholder, H_NFI_VALUE(H_VALUE_NUMBER)}, 2, dll_data_placeholder);
    define_native(stack, "dll_get_tail", h_std_dll_get_tail, (value_t[]){dll_data_placeholder, H_NFI_VALUE(H_VALUE_NUMBER)}, 2, dll_data_placeholder);
    define_native(stack, "dll_print", h_std_dll_print, (value_t[]){dll_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_NULL));
    return 1;
}
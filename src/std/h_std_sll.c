#include "h_std_sll.h"

typedef struct h_sll_node_t {
    value_t value;
    struct h_sll_node_t* next;
} h_sll_node_t;

typedef struct h_sll_t {
    h_sll_node_t* list;
    size_t size;
} h_sll_t;

static inline h_data_t* h_sll_data_make();
static void h_sll_init_data_placeholder();
static h_sll_node_t* h_sll_node_make(value_t value, h_sll_node_t* next);

static h_string_t* sll_string_placeholder;
static value_t sll_data_placeholder;

static void h_sll_init_data_placeholder() {
    sll_string_placeholder = h_string_init_hash("SLList", strlen("SLList"));
    sll_data_placeholder = (value_t){.type = H_VALUE_TYPE, .data_type = h_sll_data_make(NULL)};
}

static inline h_data_t* h_sll_data_make(h_sll_t* sll_data) {
    h_data_t* data = h_data_init(0);
    data->type_name = sll_string_placeholder;
    data->other = sll_data;
    data->size = 0;
    return data;
}

static h_sll_node_t* h_sll_node_make(value_t value, h_sll_node_t* next) {
    h_sll_node_t* node = (h_sll_node_t*)malloc(sizeof(h_sll_node_t));
    node->value = value;
    node->next = next;
    return node; 
}

value_t h_std_sll_init(struct value_t* parameters, size_t args_count) {
    h_sll_t* sll_data = (h_sll_t*)malloc(sizeof(h_sll_t));
    sll_data->list = NULL;
    sll_data->size = 0;
    return VALUE_TYPE(h_sll_data_make(sll_data));
}

value_t h_std_sll_append_head(struct value_t* parameters, size_t args_count) {
    h_sll_t* data = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_sll_node_t* node = h_sll_node_make(value, data->list);
    data->list = node;
    ++data->size;
    return NUM_VALUE(1);
}

value_t h_std_sll_append_tail(struct value_t* parameters, size_t args_count) {
    h_sll_t* data = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_sll_node_t* node = h_sll_node_make(value, NULL);
    h_sll_node_t** it = &data->list;
    for(; *it != NULL; it = &(*it)->next);
    *it = node;
    return NUM_VALUE(++data->size);
}

value_t h_std_sll_remove_first(struct value_t* parameters, size_t args_count) {
    h_sll_t* data = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_sll_node_t** it = &data->list;
    h_sll_node_t** prev = NULL;
    int index = 0;
    for(; *it != NULL; (it = &(*it)->next) && ++index) {
        if(compare_value((*it)->value, value)) {
            if(prev) {
                (*prev)->next = (*it)->next;
                --data->size;
                return NUM_VALUE(index);
            }
            data->list = (*it)->next;
            --data->size;
            return NUM_VALUE(index);
        }
        prev = it;
    }
    return NUM_VALUE(-1);
}

value_t h_std_sll_remove_last(struct value_t* parameters, size_t args_count) {
    h_sll_t* data = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_sll_node_t** it = &data->list;
    h_sll_node_t** prev = NULL;
    h_sll_node_t** pre_last = NULL;
    h_sll_node_t** last = NULL;
    int index = 0;
    int last_index = -1;
    for(; *it != NULL; (it = &(*it)->next) && ++index) {
        if(compare_value((*it)->value, value)) {
            pre_last = prev;
            last = it;
            last_index = index;
        }
        prev = it;
    }
    if(!last) return NUM_VALUE(-1);
    --data->size;
    if(!pre_last) {
        data->list = (*it)->next;
        return NUM_VALUE(index);
    }
    (*pre_last)->next = (*last)->next;
    return NUM_VALUE(index);
}

value_t h_std_sll_remove_all(struct value_t* parameters, size_t args_count) {
    h_sll_t* data = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_sll_node_t* it = data->list;
    h_sll_node_t* prev = NULL;
    h_array_t* indices = h_array_init(parameters[0].type, 5);
    int index = 0;
    for(; it != NULL; (it = it->next) && ++index) {
        if(compare_value(it->value, value)) {
            h_array_push(indices, NUM_VALUE(index));
            --data->size;            
            if(prev) {
                prev->next = it->next;
                continue;
            } 
            data->list = it->next;
        }
        prev = it;
    }
    return VALUE_ARRAY(indices);
}

value_t h_std_sll_replace_first(struct value_t* parameters, size_t args_count) {
    h_sll_t* data = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    value_t value_2 = parameters[2];
    h_sll_node_t* it = data->list;
    int index = 0;
    for(; it != NULL; (it = it->next) && ++index) {
        if(compare_value(it->value, value)) {
            it->value = value_2;
            return NUM_VALUE(index);
        }
    }
    return NUM_VALUE(-1);
}

value_t h_std_sll_replace_last(struct value_t* parameters, size_t args_count) {
    h_sll_t* data = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    value_t value_2 = parameters[2];
    h_sll_node_t* it = data->list;
    h_sll_node_t* last = NULL;
    int index = 0;
    int last_index = -1;
    for(; it != NULL; (it = it->next) && ++index) {
        if(compare_value(it->value, value)) {
            last = it;
            last_index = index;
        }
    }
    if(last_index != -1) last->value = value_2;
    return NUM_VALUE(last_index);
}

value_t h_std_sll_replace_all(struct value_t* parameters, size_t args_count) {
    h_sll_t* data = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    value_t value_2 = parameters[2];
    h_sll_node_t* it = data->list;
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

value_t h_std_sll_find_first(struct value_t* parameters, size_t args_count) {
    h_sll_t* data = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_sll_node_t* it = data->list;
    int index = 0;
    for(; it != NULL; (it = it->next) && ++index) {
        if(compare_value(it->value, value)) return NUM_VALUE(index);
    }
    return NUM_VALUE(-1);
}

value_t h_std_sll_find_last(struct value_t* parameters, size_t args_count) {
    h_sll_t* data = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_sll_node_t* it = data->list;
    int index = 0;
    int last_index = -1;
    for(; it != NULL; (it = it->next) && ++index) {
        if(compare_value(it->value, value)) {
            last_index = index;
        }
    }
    return NUM_VALUE(last_index);
}

value_t h_std_sll_find_all(struct value_t* parameters, size_t args_count) {
    h_sll_t* data = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    h_sll_node_t* it = data->list;
    int index = 0;
    h_array_t* indices = h_array_init(parameters[0].type, 5);
    for(; it != NULL; (it = it->next) && ++index) {
        if(compare_value(it->value, value)) h_array_push(indices, NUM_VALUE(index));
    }
    return VALUE_ARRAY(indices);
}

value_t h_std_sll_combine(struct value_t* parameters, size_t args_count) {
    h_sll_t* list = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    h_sll_t* list_2 = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[1])->other;
    h_sll_node_t** it = &list->list;
    for(; *it != NULL; it = &(*it)->next);
    *it = list_2->list;
    return parameters[0];
}

value_t h_std_sll_size(struct value_t* parameters, size_t args_count) {
    h_sll_t* list = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return NUM_VALUE(list->size);
}

value_t h_std_sll_get_first(struct value_t* parameters, size_t args_count) {
    h_sll_t* list = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return list->list ? list->list->value : NULL_VALUE(); 
}

value_t h_std_sll_get_last(struct value_t* parameters, size_t args_count) {
    h_sll_t* list = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    h_sll_node_t** it = &list->list;
    if(!list->list) return NULL_VALUE();
    for(; (*it)->next != NULL; it = &(*it)->next);
    return (*it)->value;
}

value_t h_std_sll_get_head(struct value_t* parameters, size_t args_count) {
    h_sll_t* list = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    unsigned int index = H_NFI_NUM_TO_CUINT(parameters[1]) + 1;
    h_sll_node_t* it = list->list;
    if(!list->list) return NULL_VALUE();
    for(; it->next != NULL && (--index > 0); it = it->next);
    return it->value;
}

value_t h_std_sll_get_tail(struct value_t* parameters, size_t args_count) {
    h_sll_t* list = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    unsigned int index = (unsigned int)list->size - H_NFI_NUM_TO_CUINT(parameters[1]) + 1;
    h_sll_node_t* it = list->list;
    if(!list->list) return NULL_VALUE();
    for(; it->next != NULL && (--index > 0); it = it->next);
    return it->value;
}

value_t h_std_sll_print(struct value_t* parameters, size_t args_count) {
    h_sll_t* data = (h_sll_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    for(h_sll_node_t* it = data->list; it != NULL; it = it->next) {
        print_value(&it->value);
    }
    return NULL_VALUE();
}

int h_std_sll_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    h_sll_init_data_placeholder();
    define_native(stack, "sll_init", h_std_sll_init, NULL, 0, sll_data_placeholder);
    define_native_data(types_table, "SLList", NULL, 0);
    define_native(stack, "sll_append_head", h_std_sll_append_head, (value_t[]){sll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NULL));
    define_native(stack, "sll_append_tail", h_std_sll_append_tail, (value_t[]){sll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "sll_remove_first", h_std_sll_remove_first, (value_t[]){sll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "sll_remove_last", h_std_sll_remove_last, (value_t[]){sll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "sll_remove_all", h_std_sll_remove_all, (value_t[]){sll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_ARRAY));
    define_native(stack, "sll_replace_first", h_std_sll_replace_first, (value_t[]){sll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC), H_NFI_VALUE(H_VALUE_GENERIC)}, 3, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "sll_replace_last", h_std_sll_replace_last, (value_t[]){sll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC), H_NFI_VALUE(H_VALUE_GENERIC)}, 3, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "sll_replace_all", h_std_sll_replace_all, (value_t[]){sll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC), H_NFI_VALUE(H_VALUE_GENERIC)}, 3, H_NFI_VALUE(H_VALUE_ARRAY));
    define_native(stack, "sll_find_first", h_std_sll_find_first, (value_t[]){sll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "sll_find_last", h_std_sll_find_last, (value_t[]){sll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "sll_find_all", h_std_sll_find_all, (value_t[]){sll_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_ARRAY));
    define_native(stack, "sll_combine", h_std_sll_combine, (value_t[]){sll_data_placeholder, sll_data_placeholder}, 2, sll_data_placeholder);
    define_native(stack, "sll_size", h_std_sll_size, (value_t[]){sll_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "sll_get_first", h_std_sll_get_first, (value_t[]){sll_data_placeholder}, 1, sll_data_placeholder);
    define_native(stack, "sll_get_last", h_std_sll_get_last, (value_t[]){sll_data_placeholder}, 1, sll_data_placeholder);
    define_native(stack, "sll_get_head", h_std_sll_get_head, (value_t[]){sll_data_placeholder, H_NFI_VALUE(H_VALUE_NUMBER)}, 2, sll_data_placeholder);
    define_native(stack, "sll_get_tail", h_std_sll_get_tail, (value_t[]){sll_data_placeholder, H_NFI_VALUE(H_VALUE_NUMBER)}, 2, sll_data_placeholder);
    define_native(stack, "sll_print", h_std_sll_print, (value_t[]){sll_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_NULL));
    return 1;
}
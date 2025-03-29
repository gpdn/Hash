#include "h_std_queue.h"

#define H_STD_DEFAULT_QUEUE_CAPACITY 4

typedef struct h_queue_t {
    value_t* data;
    value_t* start;
    size_t size;
    size_t capacity;
} h_queue_t;

static inline h_data_t* h_queue_data_make();
static void h_queue_init_data_placeholder();

static h_string_t* queue_string_placeholder;
static value_t queue_data_placeholder;

static void h_queue_init_data_placeholder() {
    queue_string_placeholder = h_string_init_hash("Queue", strlen("Queue"));
    queue_data_placeholder = (value_t){.type = H_VALUE_TYPE, .data_type = h_queue_data_make(NULL)};
}

static inline h_data_t* h_queue_data_make(h_queue_t* queue_data) {
    h_data_t* data = h_data_init(0);
    data->type_name = queue_string_placeholder;
    data->other = queue_data;
    data->size = 0;
    return data;
}

value_t h_std_queue_init(struct value_t* parameters, size_t args_count) {
    h_queue_t* queue = (h_queue_t*)malloc(sizeof(h_queue_t));
    queue->data = (value_t*)malloc(sizeof(value_t) * H_STD_DEFAULT_QUEUE_CAPACITY);
    queue->start = queue->data;
    queue->size = 0;
    queue->capacity = H_STD_DEFAULT_QUEUE_CAPACITY;
    return VALUE_TYPE(h_queue_data_make(queue));
}

value_t h_std_queue_add(struct value_t* parameters, size_t args_count) {
    h_queue_t* queue = (h_queue_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    value_t value = parameters[1];
    if(queue->size == queue->capacity) {
        queue->capacity <<= 1;
        size_t index = queue->start - queue->data;
        queue->data = (value_t*)realloc(queue->data, sizeof(value_t) * queue->capacity);
        queue->start = queue->data + index;
        if(index > 0) {
            memcpy(queue->data + queue->size + index, queue->data, sizeof(value_t) * index);
            memcpy(queue->data, queue->start, sizeof(value_t) * (queue->size - index));
            memcpy(queue->start, queue->data + queue->size + index, sizeof(value_t) * index);
            queue->start = queue->data;
        }
    }
    *(queue->data + (((queue->start - queue->data) + queue->size++) & (queue->capacity - 1))) = value;
    return NULL_VALUE();
}

value_t h_std_queue_get(struct value_t* parameters, size_t args_count) {
    h_queue_t* queue = (h_queue_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    if(queue->size == 0) return NULL_VALUE();
    --queue->size;
    value_t value = *queue->start;
    queue->start = queue->data + ((queue->start - queue->data + 1) & (queue->capacity - 1));
    return value;
}

value_t h_std_queue_peek(struct value_t* parameters, size_t args_count) {
    h_queue_t* queue = (h_queue_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return queue->size ? *queue->start : NULL_VALUE();
}

value_t h_std_queue_clear(struct value_t* parameters, size_t args_count) {
    h_queue_t* queue = (h_queue_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    queue->size = 0;
    queue->start = queue->data;
    return NULL_VALUE();
}

value_t h_std_queue_size(struct value_t* parameters, size_t args_count) {
    h_queue_t* queue = (h_queue_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return NUM_VALUE(queue->size);
}

value_t h_std_queue_print(struct value_t* parameters, size_t args_count) {
    h_queue_t* queue = (h_queue_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;    
    value_t* it = queue->start;
    for(size_t i = 1; i < queue->size + 1; ++i) {
        print_value(it);
        it = queue->data + ((queue->start - queue->data + i) & (queue->capacity - 1));
    }

    //size_t index = queue->start - queue->data;
    //for(value_t* it = queue->data; it != queue->data + queue->size; ++it) print_value(it);
    //for(value_t* it = queue->start; it != queue->start + (queue->size - (queue->start - queue->data)); ++it) print_value(it);
    //for(value_t* it = queue->start; it != queue->data + queue->capacity - index; ++it) print_value(it);
    //for(value_t* it = queue->start; it != queue->start + queue->size; ++it) print_value(it);
    
    /* printf("\n");
    if(queue->size > (queue->data + queue->capacity - queue->start)) {
        for(value_t* it = queue->start; it != queue->start + (queue->size - (queue->start - queue->data)); ++it) print_value(it);
        for(value_t* it = queue->data; it != queue->data + (queue->capacity - queue->size); ++it) print_value(it);
        return NULL_VALUE();
    }
    
    for(value_t* it = queue->start; it != queue->start + queue->size; ++it) print_value(it); */
    return NULL_VALUE();
}

int h_std_queue_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    h_queue_init_data_placeholder();
    define_native_data(types_table, "Queue", NULL, 0);
    define_native(stack, "queue_init", h_std_queue_init, NULL, 0, queue_data_placeholder);
    define_native(stack, "queue_add", h_std_queue_add, (value_t[]){queue_data_placeholder, H_NFI_VALUE(H_VALUE_GENERIC)}, 2, H_NFI_VALUE(H_VALUE_NULL));
    define_native(stack, "queue_get", h_std_queue_get, (value_t[]){queue_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_GENERIC));
    define_native(stack, "queue_peek", h_std_queue_peek, (value_t[]){queue_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_GENERIC));
    define_native(stack, "queue_clear", h_std_queue_clear, (value_t[]){queue_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_NULL));
    define_native(stack, "queue_size", h_std_queue_size, (value_t[]){queue_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "queue_print", h_std_queue_print, (value_t[]){queue_data_placeholder}, 1, H_NFI_VALUE(H_VALUE_NULL));
    return 1;
}

#undef H_STD_DEFAULT_QUEUE_CAPACITY
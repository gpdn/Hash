#include "h_std_timer.h"

#define H_STD_TIMER_DEFAULT_CAPACITY 5
#define H_STD_TIMER_TYPE (value_t){.type = H_VALUE_TYPE, .string = h_string_init_hash("Timer", strlen("Timer"))}
#define H_STD_TIMER_GET_TIME(start) NUM_VALUE(((double)clock() - H_NFI_NUM_TO_CDOUBLE(start))/((double)CLOCKS_PER_SEC))

/* static h_std_timer_t* h_std_timer_init();
static void h_std_timer_push(h_std_timer_t* timer, double num);
static void h_std_timer_free(h_std_timer_t* timer);

static h_std_timer_t* h_std_timer_init() {
    h_std_timer_t* timer = (h_std_timer_t*)malloc(sizeof(h_std_timer_t));
    timer->capacity = H_STD_TIMER_DEFAULT_CAPACITY;
    timer->size = 0;
    timer->times = (double*)malloc(sizeof(double) * H_STD_TIMER_DEFAULT_CAPACITY);
    return timer;
}

static void h_std_timer_push(h_std_timer_t* timer, double time) {
    if(timer->size >= timer->capacity) {
        timer->capacity *= 2;
        timer->times = (double*)realloc(timer->times, sizeof(double) * timer->capacity);
    }
    timer->times[timer->size++] = time;
}

static void h_std_timer_free(h_std_timer_t* timer) {
    free(timer->times);
    free(timer);
} */

value_t h_std_timer_start(struct value_t* parameters, size_t args_count) {
    h_data_t* data = h_data_init(3);
    h_string_t* type_name = h_string_init_hash("Timer", strlen("Timer"));
    data->type_name = type_name;
    data->other = NULL;
    data->size = 3;
    clock_t time = clock();
    h_array_t* array = h_array_init(H_VALUE_NUMBER, 5);
    data->data[0] = NUM_VALUE(time);
    data->data[1] = VALUE_ARRAY(array);
    data->data[2] = NUM_VALUE(time);
    return VALUE_TYPE(data);
}

value_t h_std_timer_elapsed(struct value_t* parameters, size_t args_count) {
    h_data_t* data = H_NFI_TYPE_TO_TYPE(parameters[0]);
    return H_STD_TIMER_GET_TIME(H_NFI_TYPE_GET_DATA(data, 0));
}

value_t h_std_timer_stop(struct value_t* parameters, size_t args_count) {
    h_data_t* data = H_NFI_TYPE_TO_TYPE(parameters[0]);
    value_t total_time = H_STD_TIMER_GET_TIME(data->data[0]);
    H_NFI_TYPE_SET_DATA(data, 2, total_time);
    return total_time;
}

value_t h_std_timer_lap(struct value_t* parameters, size_t args_count) {
    h_data_t* data = H_NFI_TYPE_TO_TYPE(parameters[0]);
    h_array_t* laps = H_NFI_ARR(H_NFI_TYPE_GET_DATA(data, 1));

    value_t lap_time;

    if(laps->size == 0) {
        lap_time = H_STD_TIMER_GET_TIME(H_NFI_TYPE_GET_DATA(data, 0));
        return lap_time;
    }

    lap_time = H_STD_TIMER_GET_TIME(laps->data[laps->size - 1]);
    h_array_push(laps, lap_time);
    return lap_time;
}

value_t h_std_timer_reset(struct value_t* parameters, size_t args_count) {
    h_data_t* data = H_NFI_TYPE_TO_TYPE(parameters[0]);
    H_NFI_TYPE_SET_DATA(data, 0, NUM_VALUE(0));
    h_array_free(H_NFI_ARR(H_NFI_TYPE_GET_DATA(data, 1)));
    h_array_t* array = h_array_init(H_VALUE_NUMBER, 5);
    clock_t time = clock();
    data->data[0] = NUM_VALUE(time);
    data->data[1] = VALUE_ARRAY(array);
    data->data[2] = NUM_VALUE(time);
    H_NFI_TYPE_SET_DATA(data, 2, NUM_VALUE(0));
    return NUM_VALUE(1);
}

value_t h_std_timer_free(struct value_t* parameters, size_t args_count) {
    return NUM_VALUE(1);
}

int h_std_timer_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    define_native_data(types_table, "Timer", (h_native_data_entry_t[]){
        (h_native_data_entry_t){.name = "start", .value = (value_t){.type = H_VALUE_NUMBER}},
        (h_native_data_entry_t){.name = "laps", .value = (value_t){.type = H_VALUE_ARRAY}},
        (h_native_data_entry_t){.name = "stop", .value = (value_t){.type = H_VALUE_NUMBER}}
    }, 3);
    define_native(stack, "timer_start", h_std_timer_start, NULL, 0, H_STD_TIMER_TYPE);
    define_native(stack, "timer_elapsed", h_std_timer_elapsed, (value_t[]){H_STD_TIMER_TYPE}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "timer_lap", h_std_timer_lap, (value_t[]){H_STD_TIMER_TYPE}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "timer_stop", h_std_timer_stop, (value_t[]){H_STD_TIMER_TYPE}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "timer_reset", h_std_timer_reset, (value_t[]){H_STD_TIMER_TYPE}, 1, (value_t){.type = H_VALUE_NUMBER});
    return 1;
}

#undef H_TIMER_DEFAULT_CAPACITY
#undef H_STD_TIMER_TYPE
#undef H_STD_TIMER_GET_TIME
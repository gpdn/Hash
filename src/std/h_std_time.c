

#include "h_std_time.h"

#define TM_T struct tm

typedef struct h_time_data_t {
    time_t timestamp;
    TM_T timeinfo;
} h_time_data_t;

static inline h_data_t* h_time_data_make();
static void h_time_init_data_placeholder();

static h_string_t* time_string_placeholder;
static value_t time_data_placeholder;

static void h_time_init_data_placeholder() {
    time_string_placeholder = h_string_init_hash("Time", strlen("Time"));
    time_data_placeholder = (value_t){.type = H_VALUE_TYPE, .data_type = h_time_data_make(NULL)};
}

static inline h_data_t* h_time_data_make(h_time_data_t* time_data) {
    h_data_t* data = h_data_init(0);
    data->type_name = time_string_placeholder;
    data->other = time_data;
    data->size = 0;
    return data;
}

value_t h_std_time_get_current(struct value_t* parameters, size_t args_count) {
    h_time_data_t* time_data = (h_time_data_t*)malloc(sizeof(h_time_data_t));
    time(&time_data->timestamp);
    time_data->timeinfo = *gmtime(&time_data->timestamp);
    return VALUE_TYPE(h_time_data_make(time_data));
}

value_t h_std_time_get_timestamp(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return NUM_VALUE(data->timestamp);
}

value_t h_std_time_get_year(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return NUM_VALUE(data->timeinfo.tm_year + 1900);
}

value_t h_std_time_get_month(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return NUM_VALUE(data->timeinfo.tm_mon);
}

value_t h_std_time_get_day(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return NUM_VALUE(data->timeinfo.tm_mday);
}

value_t h_std_time_get_weekday(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return NUM_VALUE(data->timeinfo.tm_wday);
}

value_t h_std_time_get_yearday(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return NUM_VALUE(data->timeinfo.tm_yday);
}

value_t h_std_time_get_hour(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return NUM_VALUE(data->timeinfo.tm_hour);
}

value_t h_std_time_get_minute(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return NUM_VALUE(data->timeinfo.tm_min);
}

value_t h_std_time_get_second(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    return NUM_VALUE(data->timeinfo.tm_sec);
}

value_t h_std_time_set_timestamp(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    data->timestamp = H_NFI_NUM_TO_CLINT(parameters[1]);
    data->timeinfo = *gmtime(&data->timestamp);
    return parameters[0];
}

value_t h_std_time_set_year(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    data->timeinfo.tm_year = H_NFI_NUM_TO_CINT(parameters[1]) - 1900;
    return parameters[0];
}

value_t h_std_time_set_month(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    data->timeinfo.tm_mon = H_NFI_NUM_TO_CINT(parameters[1]);
    return parameters[0];
}

value_t h_std_time_set_day(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    data->timeinfo.tm_mday = H_NFI_NUM_TO_CINT(parameters[1]);
    return parameters[0];
}

value_t h_std_time_set_hour(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    data->timeinfo.tm_hour = H_NFI_NUM_TO_CINT(parameters[1]);
    return parameters[0];
}

value_t h_std_time_set_minute(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    data->timeinfo.tm_min = H_NFI_NUM_TO_CINT(parameters[1]);
    return parameters[0];
}

value_t h_std_time_set_second(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    data->timeinfo.tm_sec = H_NFI_NUM_TO_CINT(parameters[1]);
    return parameters[0];
}

value_t h_std_time_update(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    data->timestamp = mktime(&data->timeinfo);
    return NUM_VALUE(data->timestamp);
}

value_t h_std_time_delta(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    h_time_data_t* data1 = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[1])->other;
    h_time_data_t* diff = (h_time_data_t*)malloc(sizeof(h_time_data_t));
    diff->timestamp = abs(data1->timestamp - data->timestamp); 
    diff->timeinfo = *gmtime(&diff->timestamp);
    return VALUE_TYPE(h_time_data_make(diff));
}

value_t h_std_time_format(struct value_t* parameters, size_t args_count) {
    h_time_data_t* data = (h_time_data_t*)H_NFI_TYPE_TO_TYPE(parameters[0])->other;
    const char* format = H_NFI_STR_TO_CSTRING(parameters[1]);
    h_string_t* formatted_time = h_string_init_hash("", 50);
    strftime(formatted_time->string, 50, format, &data->timeinfo);
    return STR_VALUE(formatted_time);
}

int h_std_time_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    h_time_init_data_placeholder();
    define_native_data(types_table, "Time", NULL, 0);
    define_native(stack, "time_get_current", h_std_time_get_current, NULL, 0, time_data_placeholder);
    define_native(stack, "time_get_timestamp", h_std_time_get_timestamp, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "time_get_year", h_std_time_get_year, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "time_get_month", h_std_time_get_month, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "time_get_day", h_std_time_get_day, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "time_get_weekday", h_std_time_get_weekday, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "time_get_yearday", h_std_time_get_yearday, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "time_get_hour", h_std_time_get_hour, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "time_get_minute", h_std_time_get_minute, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "time_get_second", h_std_time_get_second, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}}, 1, (value_t){.type = H_VALUE_NUMBER});
    define_native(stack, "time_set_timestamp", h_std_time_set_timestamp, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}, H_NFI_VALUE(H_VALUE_NUMBER)}, 2, H_NFI_VALUE(H_VALUE_NULL));
    define_native(stack, "time_set_year", h_std_time_set_year, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}, H_NFI_VALUE(H_VALUE_NUMBER)}, 2, time_data_placeholder);
    define_native(stack, "time_set_month", h_std_time_set_month, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}, H_NFI_VALUE(H_VALUE_NUMBER)}, 2, time_data_placeholder);
    define_native(stack, "time_set_day", h_std_time_set_day, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}, H_NFI_VALUE(H_VALUE_NUMBER)}, 2, time_data_placeholder);
    define_native(stack, "time_set_hour", h_std_time_set_hour, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}, H_NFI_VALUE(H_VALUE_NUMBER)}, 2, time_data_placeholder);
    define_native(stack, "time_set_minute", h_std_time_set_minute, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}, H_NFI_VALUE(H_VALUE_NUMBER)}, 2, time_data_placeholder);
    define_native(stack, "time_set_second", h_std_time_set_second, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}, H_NFI_VALUE(H_VALUE_NUMBER)}, 2, time_data_placeholder);
    define_native(stack, "time_update", h_std_time_update, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}}, 1, H_NFI_VALUE(H_VALUE_NUMBER));
    define_native(stack, "time_delta", h_std_time_delta, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}, (value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}}, 2, time_data_placeholder);
    define_native(stack, "time_format", h_std_time_format, (value_t[]){(value_t){.type = H_VALUE_TYPE, .string = time_string_placeholder}, H_NFI_VALUE(H_VALUE_STRING)}, 2, H_NFI_VALUE(H_VALUE_STRING));
    return 1;
}

#undef TM_T
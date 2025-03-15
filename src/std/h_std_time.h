#ifndef H_STD_TIME
#define H_STD_TIME

#include "h_nfi.h"
#include <time.h>

value_t h_std_time_get_current(struct value_t* parameters, size_t args_count);
value_t h_std_time_get_timestamp(struct value_t* parameters, size_t args_count);
value_t h_std_time_get_year(struct value_t* parameters, size_t args_count);
value_t h_std_time_get_month(struct value_t* parameters, size_t args_count);
value_t h_std_time_get_day(struct value_t* parameters, size_t args_count);
value_t h_std_time_get_weekday(struct value_t* parameters, size_t args_count);
value_t h_std_time_get_yearday(struct value_t* parameters, size_t args_count);
value_t h_std_time_get_hour(struct value_t* parameters, size_t args_count);
value_t h_std_time_get_minute(struct value_t* parameters, size_t args_count);
value_t h_std_time_get_second(struct value_t* parameters, size_t args_count);
value_t h_std_time_set_timestamp(struct value_t* parameters, size_t args_count);
value_t h_std_time_set_year(struct value_t* parameters, size_t args_count);
value_t h_std_time_set_month(struct value_t* parameters, size_t args_count);
value_t h_std_time_set_day(struct value_t* parameters, size_t args_count);
value_t h_std_time_set_weekday(struct value_t* parameters, size_t args_count);
value_t h_std_time_set_yearday(struct value_t* parameters, size_t args_count);
value_t h_std_time_set_hour(struct value_t* parameters, size_t args_count);
value_t h_std_time_set_minute(struct value_t* parameters, size_t args_count);
value_t h_std_time_set_second(struct value_t* parameters, size_t args_count);
int h_std_time_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif
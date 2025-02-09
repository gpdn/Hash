#ifndef H_STD_MYSQL
#define H_STD_MYSQL

#include "h_nfi.h"
#include <mysql.h>

value_t h_std_mysql_connect(struct value_t* parameters, size_t args_count);
int h_std_mysql_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table);

#endif
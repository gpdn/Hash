/* #include "h_std_mysql.h"

value_t h_std_mysql_connect(struct value_t* parameters, size_t args_count) {
    MYSQL* connection = mysql_init(NULL);
    
    if(connection == NULL) return NUM_VALUE(0);

    mysql_real_connect(connection, NULL, NULL, NULL, NULL, 0, NULL, 0);
    h_data_t* data = h_data_init(1);
    h_string_t* type_name = h_string_init_hash("MySqlConnection", strlen("MySqlConnection"));
    data->type_name = type_name;
    data->other = connection;
    data->size = 0;
    return VALUE_TYPE(data);
}

value_t h_std_mysql_start_transaction(struct value_t* parameters, size_t args_count) {
    MYSQL* connection = H_NFI_TYPE_TO_CTYPE((MYSQL*)parameters[0]);
    
    if(connection == NULL) return NUM_VALUE(0);

    return NUM_VALUE(mysql_autocommit(connection, 1));
}

value_t h_std_mysql_end_transaction(struct value_t* parameters, size_t args_count) {
    MYSQL* connection = H_NFI_TYPE_TO_CTYPE((MYSQL*)parameters[0]);
    
    if(connection == NULL) return NUM_VALUE(0);

    if(mysql_commit(connection) != 0) return NUM_VALUE(0);

    return NUM_VALUE(mysql_autocommit(connection, 0) != 0 ? 1 : 0);
}

value_t h_std_mysql_rollback_transaction(struct value_t* parameters, size_t args_count) {
    MYSQL* connection = H_NFI_TYPE_TO_CTYPE((MYSQL*)parameters[0]);
    
    if(connection == NULL) return NUM_VALUE(0);

    if(mysql_rollback(connection) != 0) return NUM_VALUE(0);

    return NUM_VALUE(mysql_autocommit(connection, 0) != 0 ? 1 : 0);
}

value_t h_std_mysql_autocommit(struct value_t* parameters, size_t args_count) {
    MYSQL* connection = H_NFI_TYPE_TO_CTYPE((MYSQL*)parameters[0]);
    int mode = H_NFI_NUM_TO_CINT(parameters[0]);
    
    if(connection == NULL) return NUM_VALUE(0);

    return NUM_VALUE(mysql_autocommit(connection, mode) != 0 ? 1 : 0);
}

value_t h_std_mysql_rollback(struct value_t* parameters, size_t args_count) {
    MYSQL* connection = H_NFI_TYPE_TO_CTYPE((MYSQL*)parameters[0]);
    
    if(connection == NULL) return NUM_VALUE(0);

    return NUM_VALUE(mysql_rollback(connection) != 0 ? 1 : 0);
}

int h_std_mysql_import(h_locals_stack_t* stack, h_ht_enums_t* enums_table, h_ht_types_t* types_table) {
    define_native_data(types_table, "MySqlConnection", (h_native_data_entry_t[]){
        {.name = "status", .value = H_NFI_VALUE(H_VALUE_NUMBER)}
    }, 1);
    define_native(stack, "mysql_connect", h_std_mysql_connect, (value_t[]){(value_t){.type = H_VALUE_STRING}}, 1, (value_t){.type = H_VALUE_STRING});
} */
#include "h_pipeline.h"

interpreter_result_t pipeline_start(const char* file_content, uint8_t flags, h_std_t* std) {
    
    clock_t pipeline_timer = timer_start_time("Pipeline");
    
    #if DEBUG_TRACE_LOG_FILE
        DEBUG_PRINT_LINE();
        DEBUG_TITLE("FILE");
        DEBUG_COLOR_SET(COLOR_BLUE);
        DEBUG_LOG("%s\n", file_content);
        DEBUG_COLOR_RESET();
        DEBUG_PRINT_LINE();
    #endif

    #if DEBUG_TIMERS
        clock_t lexer_timer = timer_start("Lexer");
    #endif

    lexer_t* lexer = lexer_init(file_content); 

    token_t* tokens_array = lexer_tokenise(lexer);
    
    #if DEBUG_TIMERS
        timer_stop_log("Lexer", lexer_timer, COLOR_CYAN);
    #endif

    #if DEBUG_TRACE_LEXER_TOKEN
        DEBUG_PRINT_LINE();
        DEBUG_COLOR_SET(COLOR_CYAN);
        DEBUG_TITLE("Tokens List");
            for(token_t* temp = tokens_array; temp->type != H_TOKEN_EOF; ++temp) {
                token_print(temp);
            }
        DEBUG_COLOR_RESET();
        DEBUG_PRINT_LINE();
    #endif

    #if DEBUG_FILE_LEXER_TOKEN && defined(DEBUG_FILE_LEXER_TOKEN_PATH)
        FILE* debug_file_lexer_token = fopen(DEBUG_FILE_LEXER_TOKEN_PATH, "wb");
        for(token_t* temp = tokens_array; temp->type != H_TOKEN_EOF; ++temp) {
            token_write_print_string(debug_file_lexer_token, temp);
        }
        fclose(debug_file_lexer_token);
    #endif

    size_t tokens_count = lexer_get_tokens_count(lexer);
    unsigned int lexer_errors_count = lexer_get_errors_count(lexer);
    unsigned int lexer_warnings_count = lexer_get_warnings_count(lexer);

    if(lexer_errors_count > 0 || lexer_warnings_count > 0) {
        if(lexer_report_tokenisation_errors(lexer) == 0) {
            free(tokens_array);
            lexer_free(lexer);
            return HASH_FAILURE;
        }
    }

    #if DEBUG_TIMERS
        clock_t parser_timer = timer_start("Parser");
    #endif

    parser_t* parser = parser_init(tokens_array, tokens_count);
    ast_node_t** ast = parser_generate_ast(parser);
    
    #if DEBUG_TIMERS
        timer_stop_log("Parser", parser_timer, COLOR_CYAN);
    #endif

    #if DEBUG_TRACE_PARSER_AST
        DEBUG_PRINT_LINE();
        DEBUG_COLOR_SET(COLOR_CYAN);
        DEBUG_TITLE("Ast Nodes List");
        for(size_t i = 0; i < parser->ast_list_size; ++i) {
            ast_print(ast[i], 0);
        }
        DEBUG_COLOR_RESET();
        DEBUG_PRINT_LINE();
    #endif

    if(parser->errors_count > 0) {
        free(tokens_array);
        lexer_free(lexer);
        parser_free(parser);
        return HASH_FAILURE;
    }

    /* #if DEBUG_FILE_PARSER_AST && defined(DEBUG_FILE_PARSER_AST_PATH)
        FILE* debug_file_parser_ast = fopen(DEBUG_FILE_PARSER_AST_PATH, "wb");
        for(size_t i = 0; i < parser->ast_list_size; ++i) {
            ast_write_print_string(ast[i], 0);
        }
        fclose(debug_file_parser_ast);
    #endif */

    h_hash_table_t* globals_table = h_hash_table_init(200, 0.75);
    h_locals_stack_t* locals_stack = h_locals_stack_init(200);
    h_ht_labels_t* labels_table = h_ht_labels_init(50, 0.75);
    h_ht_enums_t* enums_table = h_ht_enums_init(50, 0.75);
    h_ht_types_t* types_table = h_ht_types_init(200, 0.75);

    //h_std_cmd_import(locals_stack);

    h_std_resolve_imports(std, locals_stack, enums_table, types_table);

    #if DEBUG_TIMERS
        clock_t semantic_analyser_timer = timer_start("Semantic Analyser");
    #endif

    semantic_analyser_t* analyser = h_sa_init(ast, parser->ast_list_size, globals_table, locals_stack, labels_table, enums_table, types_table);
    h_sa_run(analyser);

    #if DEBUG_TIMERS
        timer_stop_log("Semantic Analyser", semantic_analyser_timer, COLOR_CYAN);
    #endif
    

    if(analyser->errors_count > 0) {
        DEBUG_LOG("Semantic analysis error");
        free(tokens_array);
        lexer_free(lexer);
        parser_free(parser);
        h_sa_free(analyser);
        return HASH_FAILURE;
    }

    #if DEBUG_TIMERS
        clock_t icg_timer = timer_start("ICG");
    #endif

    icg_t* bytecode_generator = icg_init(ast, tokens_count, globals_table, locals_stack, labels_table, enums_table, types_table);
    bytecode_store_t* store = icg_generate_bytecode(bytecode_generator);

    #if DEBUG_TIMERS
        timer_stop_log("ICG", icg_timer, COLOR_CYAN);
    #endif


    #if DEBUG_TRACE_ICG_BYTECODE
        DEBUG_PRINT_LINE();
        disassemble_bytecode_store(store, "Bytecode Store", NULL);
        DEBUG_PRINT_LINE();
    #endif

    #if DEBUG_TIMERS
        clock_t vm_timer = timer_start("VM");
    #endif
   
    virtual_machine_t* vm = vm_init(store, globals_table, locals_stack);
    interpreter_result_t result = vm_run(vm);

    #if DEBUG_TIMERS
        timer_stop_log("VM", vm_timer, COLOR_CYAN);
    #endif

    free(tokens_array);
    lexer_free(lexer);
    parser_free(parser);
    h_sa_free(analyser);
    bs_free(store);
    vm_free(vm);
    h_locals_stack_free(locals_stack);
    h_hash_table_free(globals_table);
    h_ht_labels_free(labels_table);
    h_ht_enums_free(enums_table);
    h_ht_types_free(types_table);

    timer_stop_log("Pipeline", pipeline_timer, COLOR_GREEN);

    return result;
}
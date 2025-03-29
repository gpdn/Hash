#include "h_pipeline.h"

int pipeline_start(const char* file_content, uint8_t flags, h_std_t* std) {
    
    pipeline_t pipeline = {0};

    pipeline.timer = timer_start_time("Pipeline");
    
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

    pipeline.lexer = lexer_init(file_content);
    pipeline.tokens_array = lexer_tokenise(pipeline.lexer);
    
    #if DEBUG_TIMERS
        timer_stop_log("Lexer", lexer_timer, COLOR_CYAN);
    #endif

    #if DEBUG_TRACE_LEXER_TOKEN
        DEBUG_PRINT_LINE();
        DEBUG_COLOR_SET(COLOR_CYAN);
        DEBUG_TITLE("Tokens List");
            for(token_t* temp = pipeline.tokens_array; temp->type != H_TOKEN_EOF; ++temp) {
                token_print(temp);
            }
        DEBUG_COLOR_RESET();
        DEBUG_PRINT_LINE();
    #endif

    #if DEBUG_FILE_LEXER_TOKEN && defined(DEBUG_FILE_LEXER_TOKEN_PATH)
        FILE* debug_file_lexer_token = fopen(DEBUG_FILE_LEXER_TOKEN_PATH, "wb");
        for(token_t* temp = pipeline.tokens_array; temp->type != H_TOKEN_EOF; ++temp) {
            token_write_print_string(debug_file_lexer_token, temp);
        }
        fclose(debug_file_lexer_token);
    #endif

    size_t tokens_count = lexer_get_tokens_count(pipeline.lexer);
    unsigned int lexer_errors_count = lexer_get_errors_count(pipeline.lexer);
    unsigned int lexer_warnings_count = lexer_get_warnings_count(pipeline.lexer);

    if(lexer_errors_count > 0 || lexer_warnings_count > 0) {
        if(lexer_report_tokenisation_errors(pipeline.lexer) == 0) {
            free(pipeline.tokens_array);
            lexer_free(pipeline.lexer);
            return HASH_FAILURE;
        }
    }

    #if DEBUG_TIMERS
        clock_t parser_timer = timer_start("Parser");
    #endif

    pipeline.parser = parser_init(pipeline.tokens_array, tokens_count);
    pipeline.ast = parser_generate_ast(pipeline.parser);
    
    #if DEBUG_TIMERS
        timer_stop_log("Parser", parser_timer, COLOR_CYAN);
    #endif

    #if DEBUG_TRACE_PARSER_AST
        DEBUG_PRINT_LINE();
        DEBUG_COLOR_SET(COLOR_CYAN);
        DEBUG_TITLE("Ast Nodes List");
        for(size_t i = 0; i < pipeline.parser->ast_list_size; ++i) {
            ast_print(pipeline.ast[i], 0);
        }
        DEBUG_COLOR_RESET();
        DEBUG_PRINT_LINE();
    #endif

    if(pipeline.parser->errors_count > 0) {
        pipeline_free(&pipeline);
        return HASH_FAILURE;
    }

    /* 
        put after parser->error_count

        #if DEBUG_FILE_PARSER_AST && defined(DEBUG_FILE_PARSER_AST_PATH)
            FILE* debug_file_parser_ast = fopen(DEBUG_FILE_PARSER_AST_PATH, "wb");
            for(size_t i = 0; i < parser->ast_list_size; ++i) {
                ast_write_print_string(ast[i], 0);
            }
            fclose(debug_file_parser_ast);
        #endif 
    */

    pipeline.globals_table = h_hash_table_init(200, 0.75);
    pipeline.locals_stack = h_locals_stack_init(200);
    pipeline.labels_table = h_ht_labels_init(64, 0.75);
    pipeline.enums_table = h_ht_enums_init(64, 0.75);
    pipeline.types_table = h_ht_types_init(200, 0.75);

    h_std_resolve_imports(std, pipeline.locals_stack, pipeline.enums_table, pipeline.types_table);

    size_t initial_search_index = pipeline.locals_stack->size;

    #if DEBUG_TIMERS
        clock_t semantic_analyser_timer = timer_start("Semantic Analyser");
    #endif

    pipeline.analyser = h_sa_init(pipeline.ast, pipeline.parser->ast_list_size, pipeline.globals_table, pipeline.locals_stack, pipeline.labels_table, pipeline.enums_table, pipeline.types_table);
    h_sa_run(pipeline.analyser);

    #if DEBUG_TIMERS
        timer_stop_log("Semantic Analyser", semantic_analyser_timer, COLOR_CYAN);
    #endif
    
    if(pipeline.analyser->errors_count > 0) {
        DEBUG_LOG("Semantic analysis error");
        pipeline_free(&pipeline);
        return HASH_FAILURE;
    }

    if((flags >> H_ARGS_FLAG_COMPILE) & 1) {
    
        #if DEBUG_TIMERS
           clock_t compiler_timer = timer_start("Compiler");
        #endif
        
        pipeline.compiler = h_compiler_init(pipeline.ast, pipeline.parser->ast_list_size, pipeline.locals_stack, pipeline.types_table, pipeline.enums_table);
        int result = h_compiler_run(&pipeline.compiler);
        
        if(result != 0) {DEBUG_LOG("Compilation failed");}

        DEBUG_LOG("Running in Compile Mode\n");
        #if DEBUG_TIMERS
            timer_stop_log("Compiler", compiler_timer, COLOR_CYAN);
        #endif

        if((flags >> H_ARGS_FLAG_RUN) ^ 1) {
            pipeline_free(&pipeline);
            timer_stop_log("Pipeline", pipeline.timer, COLOR_GREEN);
            return 0;
        }

        system("compiled.exe");
        return 0;

    }

    #if DEBUG_TIMERS
        clock_t icg_timer = timer_start("ICG");
    #endif

    pipeline.bytecode_generator = icg_init(pipeline.ast, tokens_count, pipeline.globals_table, pipeline.locals_stack, pipeline.labels_table, pipeline.enums_table, pipeline.types_table, initial_search_index);
    pipeline.store = icg_generate_bytecode(pipeline.bytecode_generator);

    #if DEBUG_TIMERS
        timer_stop_log("ICG", icg_timer, COLOR_CYAN);
    #endif

    #if DEBUG_TRACE_ICG_BYTECODE
        DEBUG_PRINT_LINE();
        disassemble_bytecode_store(pipeline.store, "Bytecode Store", NULL);
        DEBUG_PRINT_LINE();
    #endif

    #if DEBUG_TIMERS
        clock_t vm_timer = timer_start("VM");
    #endif

    //return 0;
   
    virtual_machine_t* vm = vm_init(pipeline.store, pipeline.globals_table, pipeline.locals_stack);
    int result = vm_run(vm);

    #if DEBUG_TIMERS
        timer_stop_log("VM", vm_timer, COLOR_CYAN);
    #endif

    pipeline_free(&pipeline);

    timer_stop_log("Pipeline", pipeline.timer, COLOR_GREEN);

    return result;
}

void pipeline_free(pipeline_t* pipeline) {
    if(pipeline->tokens_array) free(pipeline->tokens_array);
    if(pipeline->lexer) lexer_free(pipeline->lexer);
    if(pipeline->parser) parser_free(pipeline->parser);
    if(pipeline->analyser) h_sa_free(pipeline->analyser);
    if(pipeline->store) bs_free(pipeline->store);
    if(pipeline->vm) vm_free(pipeline->vm);
    if(pipeline->locals_stack) h_locals_stack_free(pipeline->locals_stack);
    if(pipeline->globals_table) h_hash_table_free(pipeline->globals_table);
    if(pipeline->labels_table) h_ht_labels_free(pipeline->labels_table);
    if(pipeline->enums_table) h_ht_enums_free(pipeline->enums_table);
    if(pipeline->types_table) h_ht_types_free(pipeline->types_table);
}
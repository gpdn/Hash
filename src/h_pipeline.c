#include "h_pipeline.h"

interpreter_result_t pipeline_start(const char* file_content, uint8_t flags) {
    
    clock_t pipeline_timer = timer_start_time("Pipeline");
    
    #if DEBUG_TRACE_LOG_FILE
        DEBUG_PRINT_LINE();
        DEBUG_TITLE("FILE");
        DEBUG_COLOR_SET(COLOR_BLUE);
        DEBUG_LOG("%s\n", file_content);
        DEBUG_COLOR_RESET();
        DEBUG_PRINT_LINE();
    #endif

    lexer_t* lexer = lexer_init(file_content); 

    token_t* tokens_array = lexer_tokenise(lexer);

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

    parser_t* parser = parser_init(tokens_array, tokens_count);
    ast_node_t** ast = parser_generate_ast(parser);

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

    icg_t* bytecode_generator = icg_init(ast, tokens_count);
    bytecode_store_t* store = icg_generate_bytecode(bytecode_generator);

    #if DEBUG_TRACE_ICG_BYTECODE
        DEBUG_PRINT_LINE();
        disassemble_bytecode_store(store, "Bytecode Store", NULL);
        DEBUG_PRINT_LINE();
    #endif
   
    virtual_machine_t* vm = vm_init(store);

    interpreter_result_t result = vm_run(vm);

    free(tokens_array);
    lexer_free(lexer);
    parser_free(parser);
    bs_free(store);
    vm_free(vm);

    timer_stop_log("Pipeline", pipeline_timer);

    return HASH_SUCCESS;
}
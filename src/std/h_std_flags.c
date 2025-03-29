#include "h_std_flags.h"

const char* h_std_resolve_import_flag(size_t flag) {
    switch(flag) {
        case H_STD_FLAG_CMD:
            return "H_STD_FLAG_CMD";
        case H_STD_FLAG_FILE:
            return "H_STD_FLAG_FILE";
        case H_STD_FLAG_STR:
            return "H_STD_FLAG_STR";
        case H_STD_FLAG_ARR:
            return "H_STD_FLAG_ARR";
        case H_STD_FLAG_TYPE:
            return "H_STD_FLAG_TYPE";
        case H_STD_FLAG_TIME:
            return "H_STD_FLAG_TIME";
        case H_STD_FLAG_TIMER:
            return "H_STD_FLAG_TIMER";
        case H_STD_FLAG_SYSTEM:
            return "H_STD_FLAG_SYSTEM";
        case H_STD_FLAG_MATH:
            return "H_STD_FLAG_MATH";
        case H_STD_FLAG_DS_SLL:
            return "H_STD_FLAG_DS_SLL";
        case H_STD_FLAG_DS_DLL:
            return "H_STD_FLAG_DS_DLL";
        case H_STD_FLAG_DS_STACK:
            return "H_STD_FLAG_DS_STACK";
        case H_STD_FLAG_DS_MAP:
            return "H_STD_FLAG_DS_MAP";
        case H_STD_FLAG_DS_QUEUE:
            return "H_STD_FLAG_DS_QUEUE";
        default:
            return "Undefined flag";
    }
}
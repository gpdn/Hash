#include "h_timer.h"

clock_t timer_start(const char* name) {
    DEBUG_COLOR_SET(COLOR_GREEN);
    DEBUG_LOG("%s started\n", name);
    DEBUG_COLOR_RESET();
    return clock();
}

clock_t timer_start_time(const char* name) {
    time_t current_time;
    struct tm* time_info; 

    char time_string[50];

    time(&current_time);
    time_info = localtime(&current_time);

    strftime(time_string, 50, "%a %d %B %Y %H:%M:%S", time_info);

    DEBUG_COLOR_SET(COLOR_GREEN);
    DEBUG_LOG("%s started at %s\n", name, time_string);
    DEBUG_COLOR_RESET();

    return clock();
}

clock_t timer_stop(clock_t timer_start) {
    return (clock() - timer_start)/CLOCKS_PER_SEC;
}

clock_t timer_stop_log(const char* name, clock_t timer_start) {
    clock_t timer_end = timer_stop(timer_start);
    DEBUG_COLOR_SET(COLOR_BLUE);
    DEBUG_LOG("%s finished in %lds\n", name, timer_end);
    DEBUG_COLOR_RESET();
    return timer_end;
}
#include "h_timer.h"

clock_t timer_start(const char* name) {
    printf("%s%s started %s\n", COLOR_CYAN, name, COLOR_RESET);
    return clock();
}

clock_t timer_start_time(const char* name) {
    time_t current_time;
    struct tm* time_info; 

    char time_string[50];

    time(&current_time);
    time_info = localtime(&current_time);

    strftime(time_string, 50, "%a %d %B %Y %H:%M:%S", time_info);

    printf("%s%s started at %s%s\n", COLOR_GREEN, name, time_string, COLOR_RESET);

    return clock();
}

float timer_stop(clock_t timer_start) {
    return (clock() - timer_start)/((float)CLOCKS_PER_SEC);
}

float timer_stop_log(const char* name, clock_t timer_start, const char* color) {
    float timer_end = timer_stop(timer_start);
    printf("%s%s finished in %1.4fs%s\n", color, name, timer_end, COLOR_RESET);
    return timer_end;
}
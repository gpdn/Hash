#ifndef HASH_TIMER_H
#define HASH_TIMER_H

#include <stdio.h>
#include <time.h>
#include "debug.h"

clock_t timer_start(const char* name);
clock_t timer_start_time(const char* name);
float timer_stop(clock_t timer_start);
float timer_stop_log(const char* name, clock_t timer_start, const char* color);

#endif
#include "Timer.h"
#include <stdio.h>

clock_t start_timer() {
    return clock();
}

void stop_timer(const clock_t start_time) {
    clock_t end_time = clock();
    double execution_time = (double) (end_time - start_time) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", execution_time);
}

void print_current_time() {
    time_t now = time(0);
    struct tm *tm;
    if ((tm = localtime(&now)) == NULL) {
        printf("Error extracting time stuff\n");
    }

    printf("%04d-%02d-%02d %02d:%02d:%02d\n",
           tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
           tm->tm_hour, tm->tm_min, tm->tm_sec);
}
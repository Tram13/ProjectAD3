#include "MemoryUsageMeter.h"
#include <sys/resource.h>
#include <stdio.h>

void measure_memory_usage() {
    // docs: https://www.man7.org/linux/man-pages/man2/getrusage.2.html
    struct rusage r_usage;
    getrusage(RUSAGE_SELF,&r_usage);
    // Print the maximum resident set size used (in kilobytes).
    printf("\nMemory usage: %ld kilobytes\n",r_usage.ru_maxrss);
}
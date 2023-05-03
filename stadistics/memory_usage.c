#include <stdio.h>
#include <stdlib.h>
#include "memory_usage.h"
#include <unistd.h>
#include <sys/resource.h>

/* Definición de la función getCurrentRSS() */
long int getCurrentRSS(void) {
    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);
    return rusage.ru_maxrss;
}

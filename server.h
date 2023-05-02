#ifndef SERVER_H
#define SERVER_H
#include <semaphore.h>

int name_counter = 0;
sem_t *name_counter_sem;

#endif

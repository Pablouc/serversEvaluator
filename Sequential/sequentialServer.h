#ifndef SEQUENTIAL_SOCKET_H
#define SEQUENTIAL_SOCKET_H
#include <semaphore.h>

struct threadParams {
  struct linkedList *list;
  struct linkedList *execList;
  struct linkedList *memList;
  sem_t *sem;
  int alive;
};

#endif

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

#include "linkedList.h"
#include "sobel.h"
#include "sockets.h"
#include "memory_usage.h"
#include "statistics.h"

int cant_requests = 0;

// struct linkedList* list
void processRequests(void *arg) {
  struct threadParams *args = (struct threadParams *)arg;
  struct linkedList *list = args->list;

  char *image_name;
  struct timespec start, end;
  long long time_elapsed;
  int mem_usage;

  // Inicializar los datos para el servidor secuencial
  struct serverData server = {0, 0, 0};

  while (args->alive || list->size > 0) {
    if (list->head != NULL) {
      // printList(list);
      struct node current = pop(list);
      image_name = malloc(sizeof(char) * 50);

      strcpy(image_name, current.image);
      printf("current: %d\n", current.image_num);

      // Obtener el tiempo de inicio de la solicitud
      clock_gettime(CLOCK_MONOTONIC_RAW, &start);

      sobel(image_name, current.image_num < 100);


      // Obtener el tiempo de finalización de la solicitud
      clock_gettime(CLOCK_MONOTONIC_RAW, &end);

      // Calcular el tiempo total de la solicitud
      time_elapsed = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;

      free(image_name);

      // Actualizar los datos para el servidor secuencial
      server.cant_solicitudes++;
      server.tiempo_ejecucion_total += time_elapsed;
      mem_usage = getCurrentRSS();
      server.consumo_memoria_total += mem_usage;
    }
  }
  // Escribir los datos en el archivo CSV
  writeCSV(server);
  pthread_exit(NULL);
}

int main() {
  struct linkedList myList;
  struct linkedList exec_time;
  struct linkedList mem_usage;

  // create a new linked list and initialize it
  initList(&myList);
  initList(&exec_time);
  initList(&mem_usage);

  pthread_t threadA, threadC;
  sem_t sem;
  sem_init(&sem, 0, 1);

  struct threadParams args = {&myList, &exec_time, &mem_usage, &sem, 1};

  pthread_create(&threadA, NULL, (void *(*)(void *)) & runSocket, &args);
  pthread_create(&threadC, NULL, (void *(*)(void *)) & processRequests, &args);

  pthread_join(threadA, NULL);
  pthread_join(threadC, NULL);

  return 0;
}

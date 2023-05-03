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
sem_t semName;

struct imageInfo {
  struct node current;
  char *imageName;
};

void processImage(void* arg){
    struct imageInfo *args = (struct imageInfo *)arg;
    char *image_name;
    printf("%s this is the image\n",(char*)args->imageName);
    image_name = malloc(sizeof(char) * 50);
    printf("This is the number %d\n",args->current.image_num);
    strcpy(image_name, args->imageName);
    sem_post(&semName);
    sobel(image_name,args->current.image_num< 100);
    return;
}


// struct linkedList* list
void processRequests(void *arg) {
  struct threadParams *args = (struct threadParams *)arg;
  struct linkedList *list = args->list;

  char *image_name;
  struct timespec start, end;
  long long time_elapsed;
  int mem_usage;

  // Inicializar los datos para el servidor hilos
  struct serverData server = {0, 0, 0, "hilos"};

  while (args->alive || list->size > 0) {
    if (list->head != NULL) {
      // printList(list);
      struct node current = pop(list);
      image_name = malloc(sizeof(char) * 50);

      strcpy(image_name, current.image);
      printf("current: %s\n", image_name);

      // Obtener el tiempo de inicio de la solicitud
      clock_gettime(CLOCK_MONOTONIC_RAW, &start);

      pthread_t Img;
      sem_wait(&semName);

      struct imageInfo args = {current,image_name};
      pthread_create(&Img, NULL, (void* (*)(void*)) &processImage, &args); 

      printf("Thread creado\n");
      pthread_join(Img, NULL);

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

  sem_init(&semName, 0, 1);
  struct threadParams args = {&myList, &exec_time, &mem_usage, &sem, 1};

  pthread_create(&threadA, NULL, (void *(*)(void *)) & runSocket, &args);
  pthread_create(&threadC, NULL, (void *(*)(void *)) & processRequests, &args);

  pthread_join(threadA, NULL);
  pthread_join(threadC, NULL);

  return 0;
}

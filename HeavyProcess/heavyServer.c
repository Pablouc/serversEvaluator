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

void processImage(int image_num, char *imageName){
    char *image_name;
    printf("%s this is the image\n",(char*)imageName);
    image_name = malloc(sizeof(char) * 50);
    strcpy(image_name, imageName);
    printf("This is the number %d\n", image_num);
    sem_post(&semName);
    sobel(image_name,image_num < 100);
    return;
}


// struct linkedList* list
void processRequests(void *arg) {
  struct threadParams *args = (struct threadParams *)arg;
  struct linkedList *list = args->list;

  char *image_name;
  struct timespec start, end;
  long long time_elapsed = 0;
  int mem_usage;

  // Inicializar los datos para el servidor secuencial
  struct serverData server = {0, 0, 0, "heavy process"};

  while (args->alive || list->size > 0) {
    if (list->head != NULL) {
      // printList(list);

      struct node current = pop(list);
      int image_num = current.image_num;
      pid_t pid;
      pid = fork();
      if(pid == 0){ 
        sem_wait(&semName);

        // Obtener el tiempo de inicio de la solicitud
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        image_name = malloc(sizeof(char) * 50);
        strcpy(image_name, current.image);
        processImage(image_num,image_name);
        free(image_name);

        // Obtener el tiempo de finalización de la solicitud
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        // Calcular el tiempo total de la solicitud
        time_elapsed = (end.tv_sec - start.tv_sec) * 1000000000 + end.tv_nsec - start.tv_nsec;


        exit(0);
      }
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

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
  while (args->alive || list->size > 0) {
    if (list->head != NULL) {
      // printList(list);
      struct node current = pop(list);
      image_name = malloc(sizeof(char) * 50);

      strcpy(image_name, current.image);
      printf("current: %s\n", image_name);
      pthread_t Img;
      sem_wait(&semName);

      struct imageInfo args = {current,image_name};
      pthread_create(&Img, NULL, (void* (*)(void*)) &processImage, &args); 

      printf("Thread creado\n");
      pthread_join(Img, NULL);

      free(image_name);
    }
  }
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

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
  int imageNum;
  char *imageName;
};

void processImage(int imageNum, char *imageName){
    char *image_name;
    printf("%s this is the image\n",(char*)imageName);
    image_name = malloc(sizeof(char) * 50);
    strcpy(image_name, imageName);
    sem_post(&semName);
    sobel(image_name,imageNum < 100);
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
      
      pid_t pid;
      pid = fork();
      if(pid == 0){ 
        image_name = malloc(sizeof(char) * 50);
        strcpy(image_name, current.image);
        printf("current: %s\n", image_name);
        sem_wait(&semName);
        processImage(current.image_num,image_name);
        free(image_name);
        exit(0);
      }
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

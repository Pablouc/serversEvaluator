#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

#include "../mesh/include/mesh.h"
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

void run_child(void *shm_ptr, int childId) {
  struct process *this_process;
  struct shm_context *context = get_shm_context(shm_ptr);
  while(get_heartbeat(shm_ptr)) {
    this_process = get_process_by_id(*(context->list), childId);
    sem_wait(&this_process->go);

    sobel(this_process->image, this_process->image_num < 100);

    sem_post(&context->available_slots);
  }
  exit(0);
}

// struct linkedList* list
void processRequests(void *arg) {
  struct threadParams *args = (struct threadParams *)arg;
  struct linkedList *list = args->list;
  void* shm_ptr = args->shm_ptr;
  pid_t pids[args->children_number - 1];

  for(int i=0; i < args->children_number; i++){
    pids[i] = fork();
    if (pids[i] == 0) {
        int childID = getpid();
        struct process *this_process = malloc(sizeof(struct process));
        sem_init(&this_process->go, 1, 0);
        this_process->id = childID;

        struct shm_context *context = get_shm_context(shm_ptr);
        add_node(context->list, this_process);

        run_child(shm_ptr, childID);

    } else if (pids[i] < 0) {
        printf("Child process was not executed");
    }
  }

  struct process * process_to_run;
  struct shm_context *context = get_shm_context(shm_ptr);
  while (args->alive || list->size > 0) {
    if (list->head != NULL) {
      sem_wait(get_available_slots_sem(shm_ptr));
      struct node current = pop(list);

      process_to_run = (*(context->list))->data;
      remove_node(context->list, 0);
      
      strcpy(
        process_to_run->image,
        current.image
      );
      process_to_run->image_num = current.image_num;
      sem_post(&process_to_run->go);
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

  int children_number;
  printf("Enter the number of child processes");
  scanf("%d", &children_number);

  mesh_initialize(children_number);
  void *shm_ptr = mesh_register_pre_heavy_process();
  
  pthread_t threadA, threadC;
  sem_t sem;
  sem_init(&sem, 0, 1);

  sem_init(&semName, 0, 1);
  struct threadParams args = {&myList, &exec_time, &mem_usage, &sem, 1, shm_ptr, children_number};

  pthread_create(&threadA, NULL, (void *(*)(void *)) & runSocket, &args);
  pthread_create(&threadC, NULL, (void *(*)(void *)) & processRequests, &args);

  pthread_join(threadA, NULL);
  pthread_join(threadC, NULL);

  return 0;
}

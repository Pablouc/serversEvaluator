#include "include/mesh.h"
#include "include/mesh_emitter.h"
#include "include/mesh_initializer.h"

#define SHM_NAME "/shared_mem"

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "./common/structures.c"
#include "include/shmem_handler.h"

#define TEST_FILE_LENGTH 20



typedef struct thread_list_node {
  pthread_t *thread_id;
  struct thread_list_node *next;
} thread_list_node_t;

thread_list_node_t *thread_list;

void add_thread_to_list(pthread_t *thread) {
  thread_list_node_t *new_node = malloc(sizeof(thread_list_node_t));
  new_node->thread_id = thread;
  new_node->next = thread_list;
  thread_list = new_node;
}

struct shm_context *get_shm_context(void *shm_ptr) {
  return (struct shm_context *)shm_ptr;
}

void *initialize_shared_memory(size_t size) {
  int shm_id = open_shared_memory(PROJECT_ID, size);
  void *shm_ptr = get_ptr_to_shared_memory(shm_id, size);
  get_shm_context(shm_ptr)->shm_id = shm_id;
  return shm_ptr;
}

void unmap_shared_memory(void *shm_ptr) {
  size_t size = sizeof(struct shm_context)+ sizeof(struct process)*get_shm_context(shm_ptr)->child_num;
  shm_unmap(shm_ptr, size);
}

int get_heartbeat(void *shm_ptr) {
  struct shm_context *context = get_shm_context(shm_ptr);
  return context->heartbeat;
}

void *mesh_get_shm_ptr() {
   void *shm_ptr = initialize_shared_memory(1);
   struct shm_context *context = get_shm_context(shm_ptr);
  int initial_shm_size = sizeof(struct shm_context) + sizeof(struct process) * context->child_num;;
  void *shm_ptr = initialize_shared_memory(initial_shm_size);
  return shm_ptr;
}


int close_shared_memory(int shm_id) {
  return shmem_close_shared_memory(shm_id);
}

struct process *get_process_list_info(void *shm_ptr)
{
    struct process *process_list_info =
        shm_ptr + sizeof(struct shm_context);
    return process_list_info;
}

void *mesh_register_pre_heavy_process()
{
    void *shm_ptr = mesh_get_shm_ptr();
    struct shm_context *context = get_shm_context(shm_ptr);
    if (context->heartbeat == 0)
    {
        printf("Error! Heartbeat is 0, mesh was not initialized!\n");
        int *errcode = malloc(sizeof(int));
        *errcode = -1;
        shm_ptr = errcode;
        return shm_ptr;
    }

    return shm_ptr;
}

void initialize_context(void *shm_ptr, int shm_id, int child_num)
{
    struct shm_context context = {
        .heartbeat = 0,
        .shm_id = shm_id,
        .child_num = child_num,
    };
    memcpy(shm_ptr, &context, sizeof(struct shm_context));
}

void initialize_processes(void *shm_ptr){
    struct shm_context *context= get_shm_context(shm_ptr);
    int child_number = context->child_num;

    struct process processes_array[child_number];

    for(int i=0; i<child_number;i++){
        sem_init(&(processes_array[i].go), 1, 0);
    }

    
    
    memcpy(
        shm_ptr + sizeof(struct shm_context),
        &processes_array,
        sizeof(processes_array));
}



void initialize_heartbeat(void *shm_ptr) {
  struct shm_context *context = get_shm_context(shm_ptr);
  context->heartbeat = 1;
}

void *mesh_initialize(int child_num)
{
    printf("Starting mesh initialization\n");
    int shm_size =
        sizeof(struct shm_context) + sizeof(struct process)*child_num;
    void *shm_ptr = initialize_shared_memory(shm_size);
    int shm_id = get_shm_context(shm_ptr)->shm_id;
    // int input_file_size = file_length;

    initialize_context(shm_ptr, shm_id, child_num);
    printf("Context initialized\n");

    initialize_heartbeat(shm_ptr);
    printf("Heartbeat initialized\n");

    initialize_processes(shm_ptr);
    printf("Processes initialized\n");

    printf("Mesh initialized correctly!\n");

    return shm_ptr;
}

int set_children_id(void *shm_ptr, int processID, int index ){
    struct shm_context *context= get_shm_context(shm_ptr);
    int child_number = context->child_num;

    struct process* process_array=  (struct process*)(shm_ptr + sizeof(struct shm_context));

    for(int i=0; i<child_number;i++){
      if(index == i){
        (process_array[i].go);
        return 1;
      }
    }
    printf("Index not found");
    return 0;
}


sem_t *mesh_get_sobel_semaphore(void *shm_ptr, int processID)
{   
    struct shm_context *context= get_shm_context(shm_ptr);
    int child_number = context->child_num;

    struct process* process_array=  (struct process*)(shm_ptr + sizeof(struct shm_context));

    for(int i=0; i<child_number;i++){
      if(process_array[i].id == processID){
        return &(process_array[i].go);
      }
     
    }
    printf("Semaphore not found");
    return NULL;
}

void mesh_finalize(void *shm_ptr) {
  struct shm_context *context = get_shm_context(shm_ptr);

  context->heartbeat = 0;

  int shm_id = context->shm_id;
  close_shared_memory(shm_id);
}

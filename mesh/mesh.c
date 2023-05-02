#include "include/mesh.h"
#include "include/mesh_emitter.h"
#include "include/mesh_initializer.h"

#define SHM_NAME "/shared_mem"

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
  size_t size = sizeof(struct shm_context);
  shm_unmap(shm_ptr, size);
}

int get_heartbeat(void *shm_ptr) {
  struct shm_context *context = get_shm_context(shm_ptr);
  return context->heartbeat;
}

void *mesh_get_shm_ptr() {
  int initial_shm_size = sizeof(struct shm_context);
  void *shm_ptr = initialize_shared_memory(initial_shm_size);
  return shm_ptr;
}

void create_thread(void *(*thread_function)(void *), void *shm_ptr) {
  pthread_t *thread = malloc(sizeof(pthread_t));
  pthread_create(thread, NULL, thread_function, shm_ptr);
  add_thread_to_list(thread);
}

int close_shared_memory(int shm_id) {
  return shmem_close_shared_memory(shm_id);
}

void initialize_context(void *shm_ptr, int buffer_size, int input_file_size,
                        int shm_id) {
  struct shm_context context = {
      .size_of_buffer = buffer_size,
      .heartbeat = 0,
      .shm_id = shm_id,
  };
  memcpy(shm_ptr, &context, sizeof(struct shm_context));
}

void initialize_heartbeat(void *shm_ptr) {
  struct shm_context *context = get_shm_context(shm_ptr);
  context->heartbeat = 1;
}

void *mesh_initialize(int buffer_size, int file_length) {
  printf("Starting mesh initialization\n");
  int shm_size = sizeof(struct shm_context);
  void *shm_ptr = initialize_shared_memory(shm_size);
  int shm_id = get_shm_context(shm_ptr)->shm_id;
  int input_file_size = file_length;

  initialize_context(shm_ptr, buffer_size, input_file_size, shm_id);
  printf("Context initialized\n");

  initialize_heartbeat(shm_ptr);
  printf("Heartbeat initialized\n");

  printf("Mesh initialized correctly!\n");

  return shm_ptr;
}

struct shm_caracter *get_buffer(void *shm_ptr) {
  struct shm_caracter *buffer =
      (struct shm_caracter *)(shm_ptr + sizeof(struct shm_context));
  return buffer;
}

struct mesh_semaphores *mesh_get_all_semaphores(void *shm_ptr) {
  struct mesh_semaphores *semaphores =
      (struct mesh_semaphores *)(shm_ptr + sizeof(struct shm_context));
  return semaphores;
}

void mesh_finalize(void *shm_ptr) {
  struct shm_context *context = get_shm_context(shm_ptr);

  context->heartbeat = 0;

  int shm_id = context->shm_id;
  close_shared_memory(shm_id);
}

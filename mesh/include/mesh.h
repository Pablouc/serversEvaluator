#ifndef MESH_H
#define MESH_H

#include <semaphore.h>
#include <stdlib.h>

#define PROJECT_ID 42

struct shm_context {
  int size_of_buffer;
  int heartbeat;
  int shm_id;
};

void *initialize_shared_memory(size_t size);
void unmap_shared_memory(void *shm_ptr);

void *mesh_initialize();
struct shm_context *get_shm_context(void *shm_ptr);
void *mesh_get_shm_ptr();
int get_heartbeat(void *shm_ptr);

void mesh_finalize(void *shm_ptr);
int close_shared_memory(int shm_id);

#endif

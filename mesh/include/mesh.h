#ifndef MESH_H
#define MESH_H

#include <semaphore.h>
#include <stdlib.h>
#include "../common/mesh_linkedList.h"

#define PROJECT_ID 42

struct shm_context {
    int heartbeat;
    int shm_id;
    int child_num;
    sem_t available_slots;
    struct mesh_node **list;
};

struct process{
    int image_num;
    sem_t go;
    int id;
    char* image;
};

void *initialize_shared_memory(size_t size);
void rotate_array(void *shm_ptr);

void unmap_shared_memory(void *shm_ptr);
sem_t *get_available_slots_sem(void * shm_ptr);
struct process *get_process_array(void * shm_ptr);

int get_heartbeat(void *shm_ptr);

void *mesh_get_shm_ptr();

int close_shared_memory(int shm_id);

struct process *get_process_list_info(void *shm_ptr);

void *mesh_register_pre_heavy_process();

void initialize_processes(void *shm_ptr);

void initialize_heartbeat(void *shm_ptr);

void *mesh_initialize(int child_num);

int set_children_id(void *shm_ptr, int processID, int index );

sem_t *mesh_get_sobel_semaphore(void *shm_ptr, int processID);

void initialize_context(void *shm_ptr, int shm_id, int child_num);

struct shm_context *get_shm_context(void *shm_ptr);

void mesh_finalize(void *shm_ptr);


#endif

#ifndef MESH_EMITTER_H
#define MESH_EMITTER_H
#include <semaphore.h>
#include <stdlib.h>

void *mesh_register_emitter();
void mesh_natural_death_emitter();
void mesh_finalize_emitter(void *shm_ptr);
int mesh_get_file_idx(void *shm_ptr);
struct shm_caracter mesh_add_caracter(void *shm_ptr,
                                      struct shm_caracter caracter);
int get_heartbeat(void *shm_ptr);

#endif

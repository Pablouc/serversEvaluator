#ifndef COMMON_STRUCTURES
#define COMMON_STRUCTURES

#include <time.h>
#include <semaphore.h>

#define PROJECT_ID 42

struct shm_context {
    int heartbeat;
    int shm_id;
    int child_num;
};

struct process{
    char* image;
    sem_t go;
    int id;
};


#endif

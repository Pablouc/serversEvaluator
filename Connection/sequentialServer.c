#include<stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/resource.h>

#include "./linkedList.c"
#include "./server.c"
#include "./sobel.c"


struct threadParams{
    struct linkedList* list;
    struct linkedList* execList;
    struct linkedList* memList;
    sem_t* sem;
};

int cant_requests = 0;




//struct linkedList* list
int addRequests(void* arg){
    struct threadParams* args = (struct threadParams*) arg;
    sem_t* sem = args->sem;
    struct linkedList* list = args->list;
    
    
    while(1){
        //verify if there is another request in the server
        //counter is a variable from server.c
        //printf("cant_requests:  %d \n", cant_requests);
        //printf("counter: %d \n",counter);
        if(cant_requests < counter){
            cant_requests= cant_requests +1;
            char imgAddr[50];
            sprintf(imgAddr, "imagenrecibida%d.jpg", cant_requests-1);

            sleep(1);
            //verify if the file exists
            FILE* image_file = fopen(imgAddr, "rb");
            if (image_file == NULL) {
                printf("Failed to open image file!\n");
                return 1;
            }

            // Wait for the semaphore before accessing the list
            sem_wait(sem);
            insertAtBeginning(list, imgAddr);
            // Release the semaphore
            printList(list);
            sem_post(sem);
            sleep(1);
        }

        
    }
    
    

    return 0;

    
}
//struct linkedList* list
void processRequests(void* arg){
    struct threadParams* args = (struct threadParams*) arg;
    sem_t* sem = args->sem;
    struct linkedList* list = args->list;
    struct linkedList* execTime = args->execList;
    struct linkedList* memUsage = args->memList;

    printList(list);
    //sleep(2);
   
   

    while(1){
        // Wait for the semaphore before accessing the list
        sem_wait(sem);


        void* ptr = malloc(1000);
        //sleep(3);
        struct node* current = list->head;
        if(current != NULL){
            printf("current: %s\n",current->image);
        }
        while(current != NULL){

           
            //To measure execution time
            struct timespec start_time, end_time;
            clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);
            // Measure the memory usage before the execution
            // Get the resource usage before the portion of code
            struct rusage before;
            getrusage(RUSAGE_SELF, &before);

            //Execution
            char* firstElem = current->image;
            sobel(firstElem);
            removeFromBeginning(list);
            current = list->head;
            printList(list);

            //Measure the memory usage after the execution
            struct rusage after;
            getrusage(RUSAGE_SELF, &after);

            /*
            // Calculate the difference in maximum resident set size
            long diff = (after.ru_maxrss - before.ru_maxrss) * 1024;
            insertAtBeginning(memUsage, diff);
            printf("Memory usage: %ld bytes\n", diff);

            // Get the end time
            clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);  
            long long elapsed_ns = (end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);  // Calculate the elapsed time in nanoseconds
            insertAtBeginning(execTime, elapsed_ns);
            
            
            printf("Elapsed time: %lld ns\n", elapsed_ns);
            sleep(1);
            */
            

        }
        // Release the semaphore
        sem_post(sem);
        free(ptr);
    }
    
    //printList(execTime);
    //printList(memUsage);

    
}

//sobel("imagenrecibida0.jpg");

int main(){
    struct linkedList myList;
    struct linkedList exec_time;
    struct linkedList mem_usage;
    
    // create a new linked list and initialize it
    initList(&myList);
    initList(&exec_time);
    initList(&mem_usage);

    pthread_t threadA, threadB, threadC;
    sem_t sem;
    sem_init(&sem, 0, 1);


    struct threadParams args={&myList,&exec_time, &mem_usage, &sem};

    pthread_create(&threadA, NULL, (void* (*)(void*)) &runSocket, NULL);
    pthread_create(&threadB, NULL, (void* (*)(void*)) &addRequests, &args);
    pthread_create(&threadC, NULL, (void* (*)(void*)) &processRequests, &args);

    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);

    return 0;
}



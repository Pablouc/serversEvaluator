#include<stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include  "./factorial.c"
#include "./linkedList.c"


struct threadParams{
    struct linkedList* list;
    sem_t* sem;
};




//struct linkedList* list
void createRequests(void* arg){
    struct threadParams* args = (struct threadParams*) arg;
    sem_t* sem = args->sem;
    struct linkedList* list = args->list;

    // Wait for the semaphore before accessing the list
    sem_wait(sem);

    insertAtBeginning(list, 4);
    printf("4 added\n");
    sleep(1);
    
    insertAtBeginning(list, 5);
    printf("5 added\n");
    sleep(1);

    insertAtBeginning(list, 7);

    
    // Release the semaphore
    sem_post(sem);

    
}
//struct linkedList* list
void processRequests(void* arg){

    struct threadParams* args = (struct threadParams*) arg;
    sem_t* sem = args->sem;
    struct linkedList* list = args->list;

    // Wait for the semaphore before accessing the list
    sem_wait(sem);

    printList(list);
    //sleep(2);

    struct node* current = list->head;
    while(current != NULL){
        int firstElem = current->data;
        factorial(firstElem);
        removeFromBeginning(list);
        current = list->head;
        printList(list);
        sleep(1);

    }

    // Release the semaphore
    sem_post(sem);
}



int main(){
    struct linkedList myList;
    
    // create a new linked list and initialize it
    initList(&myList);
    pthread_t threadA, threadB, threadC;
    sem_t sem;
    sem_init(&sem, 0, 1);

/*
    struct threadParams{
        struct linkedList myList;
        sem_t* sem;
    };*/

    struct threadParams args={&myList, &sem};

    pthread_create(&threadA, NULL, (void* (*)(void*)) &createRequests, &args);
    pthread_create(&threadB, NULL, (void* (*)(void*)) &processRequests, &args);

    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);

    return 0;
}



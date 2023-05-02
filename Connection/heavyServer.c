#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/wait.h>

#include "./server.c"
#include "./sobel.c"

int cant_requests = 0;

//struct linkedList* list
int addRequests(){
    
    
    while(1){
        //verify if there is another request in the server
        //counter is a variable from server.c
        //printf("cant_requests:  %d \n", cant_requests);
        //printf("counter: %d \n",counter);
        if(cant_requests < counter ){
            //cant_requests= cant_requests +1;
            char imgAddr[50];
            sprintf(imgAddr, "imagenrecibida%d.jpg", cant_requests++);

            sleep(1);
            //verify if the file exists
            FILE* image_file = fopen(imgAddr, "rb");
            if (image_file == NULL) {
                printf("Error------------ %s",imgAddr);
                printf("\n");
                printf("Failed to open image file!\n");
                return 1;
            }
            
            
            pid_t pid;
            pthread_t tid;
            pid = fork();
            if(pid == 0){
                //char *imageName[50]= {imgAddr}; 
                char* imageName;
                imageName = malloc(strlen(imgAddr) + 1);
                sprintf(imageName, "%s", imgAddr);
                //strcpy(imageName, imgAddr);
                pthread_create(&tid, NULL, (void *(*)(void *))sobel, imageName); // create a thread to perform sum
                cant_requests= cant_requests+1;
                pthread_exit(NULL);
                pthread_join(tid, NULL); // wait for the thread to finish
                exit(0);
            }

        }
    // wait for all child processes to finish
    for (int i = 0; i < cant_requests; i++) {
        wait(NULL);
    }


        
    }
    
    

    return 0;

    
}

int main(){
    pthread_t socket, processing;

    pthread_create(&socket, NULL, (void *(*)(void *))runSocket, NULL);
    pthread_create(&processing, NULL, (void *(*)(void *))addRequests, NULL);


    pthread_join(socket, NULL); 
    pthread_join(processing, NULL); 
}
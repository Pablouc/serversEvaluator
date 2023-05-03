#ifndef SOCKETS_H
#define SOCKETS_H
#include "linkedList.h"
#include <arpa/inet.h>
#include <semaphore.h>

#define CHUNK_SIZE 1024

struct threadParams {
  struct linkedList *list;
  struct linkedList *execList;
  struct linkedList *memList;
  sem_t *sem;
  int alive;
  void *shm_ptr;
  int children_number;
};

int counter = 0;
sem_t *socket_sem;

void runSocket(void *args);
void receive_image(int sockfd, struct linkedList *list);
int create_socket(char *ip, char *port, struct sockaddr_in *serv_addr);
int send_to_socket(int sockfd, char *buffer, int n);
int receive_from_socket(int sockfd, char *buffer, int n);
void connect_socket(int sockfd, struct sockaddr_in serv_addr);
int accept_incoming_connection(int sockfd, struct sockaddr *serv_addr);
void close_socket(int sockfd);
void send_acknowledge(int sockfd);
void wait_acknowledge_from_socket(int sockfd, char code_to_send);

#endif

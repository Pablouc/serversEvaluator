#ifndef SOCKETS_H
#define SOCKETS_H
#include <arpa/inet.h>
#include <semaphore.h>

sem_t *socket_sem;

int create_socket(char *ip, char *port, struct sockaddr_in *serv_addr);
int send_to_socket(int sockfd, char *buffer, int n);
int receive_from_socket(int sockfd, char *buffer, int n);
void connect_socket(int sockfd, struct sockaddr_in serv_addr);
int accept_incoming_connection(int sockfd, struct sockaddr *serv_addr);
void close_socket(int sockfd);
void send_acknowledge(int sockfd);
void wait_acknowledge_from_socket(int sockfd, char code_to_send);

#endif

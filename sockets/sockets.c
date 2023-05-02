#include "include/sockets.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXIMUM_CONNECTIONS 1
#define ACK_CHAR 'o'

void bind_socket(int socketfd, struct sockaddr *serv_addr, int addrlen) {
  if (bind(socketfd, serv_addr, addrlen) < 0) {
    printf("Error al bindear el socket\n");
    exit(1);
  }
}

void close_socket(int sockfd) {
  close(sockfd);
  sem_destroy(socket_sem);
  free(socket_sem);
}

int create_socket(char *ip, char *port, struct sockaddr_in *serv_addr) {
  socket_sem = malloc(sizeof(sem_t));
  sem_init(socket_sem, 0, 1);
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd <= 0) {
    printf("Error al crear el socket\n");
    exit(1);
  }

  int opt = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    printf("Error al setear las opciones del socket\n");
    exit(1);
  }

  serv_addr->sin_family = AF_INET;
  serv_addr->sin_port = htons(atoi(port));
  inet_pton(AF_INET, ip, &serv_addr->sin_addr);

  int flags = fcntl(sockfd, F_GETFL, 0);
  flags &= ~O_NONBLOCK;
  fcntl(sockfd, F_SETFL, flags);

  return sockfd;
}

int send_to_socket(int sockfd, char *buffer, int n) {
  int result = send(sockfd, buffer, n, 0);
  return result;
}

int receive_from_socket(int sockfd, char *buffer, int n) {
  // This will block execution if there is nothing to be read
  int message_len = recv(sockfd, buffer, n, 0);
  return message_len;
}

void connect_socket(int sockfd, struct sockaddr_in serv_addr) {
  int timeout_seconds;
  int server_is_not_connected = 1;
  printf("Attempting connection to server\n");
  while (server_is_not_connected) {
    timeout_seconds = 1;
    // Attempt to connect to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) ==
        0) {
      server_is_not_connected = 0;
    } else {
      // If connection failed, wait before retrying
      printf("Connection attempt failed, retrying in %d seconds...\n",
             timeout_seconds);
      sleep(timeout_seconds);
    }
  }
  printf("Established connection to server\n");
}

int accept_incoming_connection(int sockfd, struct sockaddr *serv_addr) {
  int serv_len = sizeof(*serv_addr);
  bind_socket(sockfd, serv_addr, serv_len);
  printf("Esperando cliente\n");

  if (listen(sockfd, MAXIMUM_CONNECTIONS) < 0) {
    printf("Error al escuchar el socket\n");
    exit(1);
  }

  int newsockfd = accept(sockfd, serv_addr, (socklen_t *)&serv_len);
  if (newsockfd < 0) {
    printf("Error al aceptar la conexión entrante\n");
    exit(1);
  }

  int flags = fcntl(newsockfd, F_GETFL, 0);
  flags &= ~O_NONBLOCK;
  fcntl(sockfd, F_SETFL, flags);

  return newsockfd;
}

void wait_acknowledge_from_socket(int sockfd, char code_to_send) {
  char msg_buffer[1];
  msg_buffer[0] = code_to_send;
  while (msg_buffer[0] != ACK_CHAR) {
    send_to_socket(sockfd, msg_buffer, 1);
    receive_from_socket(sockfd, msg_buffer, 1);
  }
}

void send_acknowledge(int sockfd) {
  char msg_buffer[1];
  msg_buffer[0] = ACK_CHAR;
  send_to_socket(sockfd, msg_buffer, 1);
}

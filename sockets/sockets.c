#include "include/sockets.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXIMUM_CONNECTIONS 1
#define ACK_CHAR 'o'

void runSocket(void *args) {
  int sockfd;
  struct sockaddr_in serv_addr;
  struct threadParams *params = (struct threadParams *)args;
  struct linkedList *list = params->list;

  // Acepta conexiones entrantes
  // Crea el socket
  printf("Creando el socket\n");
  sockfd = create_socket("127.0.0.1", "8080", &serv_addr);
  printf("Socket creado, esperando conexiones\n");
  sockfd = accept_incoming_connection(sockfd, (struct sockaddr *)&serv_addr);

  char *operation = malloc(3);
  while (1) {
    receive_from_socket(sockfd, operation, 1);
    printf("Operacion recibida: %c\n", *operation);
    if (*operation == '^') {
      send_acknowledge(sockfd);
      printf("Recibiendo imagen\n");
      receive_image(sockfd, list);
      printf("Preparado para la siguiente\n");
    } else if (*operation == '#') {
      printf("Cerrando el servidor\n");
      send_acknowledge(sockfd);
      break;
    }
    *operation = 0;
  }

  // Cierra el socket principal
  close(sockfd);
  params->alive = 0;

  return;
}
void receive_image(int sockfd, struct linkedList *list) {
  FILE *fp;
  char buffer[CHUNK_SIZE];

  // Crea un archivo para guardar la imagen recibida
  // TODO: Cap the file quantity
  char image_name[50];
  if (counter < 100)
    sprintf(image_name, "imagenrecibida%d.jpg", counter++);
  else
    sprintf(image_name, "imagenrecibida100.jpg");
  fp = fopen(image_name, "wb");
  if (fp == NULL) {
    printf("Error al crear el archivo de imagen\n");
    exit(1);
  }

  char size_str[32];
  int bytes_received = receive_from_socket(sockfd, size_str, 32);
  size_str[bytes_received] = '\0';
  long filesize = strtol(size_str, NULL, 10);
  printf("We will receive a file of %ld bytes\n", filesize);

  int bytes_received_total = 0;
  while (bytes_received_total < filesize) {
    size_t bytes_received = receive_from_socket(sockfd, buffer, CHUNK_SIZE);
    if (bytes_received == -1) {
      perror("Error receving\n");
      exit(1);
    }

    size_t bytes_written = fwrite(buffer, 1, bytes_received, fp);
    if (bytes_written != bytes_received) {
      perror("Error writing\n");
      exit(1);
    }

    bytes_received_total += bytes_received;
    send_acknowledge(sockfd);
  }
  printf("Whole file received!\n");
  receive_from_socket(sockfd, buffer, 1);
  send_acknowledge(sockfd);

  fclose(fp);

  printf("Agregando imagen a la cola\n");
  insertAtEnd(list, image_name);

  return;
}

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

#include "linkedList.h"
#include "sockets.h"

#include "sequentialServer.h"
#include <netinet/in.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define CHUNK_SIZE 1024

sem_t sem;
int counter = 0;

void receive_image(int sockfd, struct linkedList *list) {
  FILE *fp;
  char buffer[CHUNK_SIZE];

  // Crea un archivo para guardar la imagen recibida
  if (counter == 100) {
    return;
  }
  char image_name[50];
  sprintf(image_name, "imagenrecibida%d.jpg", counter++);
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

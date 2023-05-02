#include <arpa/inet.h>
#include <getopt.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "sockets.h"

#define CHUNK_SIZE 1024

struct thread_info {
  int sockfd;
  char *image_name;
  int n_ciclos;
};

void send_image(int sockfd, char *image_name) {
  sem_wait(socket_sem);
  printf("Enviando imagen!\n");
  FILE *fp;
  char buffer[CHUNK_SIZE];
  int n;

  // Abre el archivo de imagen
  fp = fopen(image_name, "rb");
  if (fp == NULL) {
    printf("Error al abrir el archivo de imagen\n");
    exit(1);
  }

  // Lee el archivo de imagen y envía los chunks al servidor
  wait_acknowledge_from_socket(sockfd, '^');

  fseek(fp, 0, SEEK_END);
  long filesize = ftell(fp);
  rewind(fp);

  char size_str[32];
  sprintf(size_str, "%ld", filesize);

  if (send_to_socket(sockfd, size_str, strlen(size_str)) < 0) {
    printf("Error al enviar el tamaño de la imagen\n");
    exit(1);
  }

  while ((n = fread(buffer, 1, CHUNK_SIZE, fp)) > 0) {
    if (send_to_socket(sockfd, buffer, n) < 0) {
      printf("Error al enviar los datos al servidor\n");
      exit(1);
    }
    receive_from_socket(sockfd, buffer, 1);
  }

  printf("Toda la imagen enviada...\n");
  wait_acknowledge_from_socket(sockfd, '%');

  // Cierra el archivo
  fclose(fp);
  printf("Imagen enviada\n");
  sem_post(socket_sem);
}

void *send_images(void *arg) {
  struct thread_info *info = (struct thread_info *)arg;
  for (int i = 0; i < info->n_ciclos; i++) {
    send_image(info->sockfd, info->image_name);
  }
  return NULL;
}

void create_threads(int sockfd, char *image, int n_threads, int n_ciclos) {
  // int total_ciclos = 0; // declaración y asignación del contador total_ciclos
  //  Crea los hilos
  pthread_t threads[n_threads];
  for (int i = 0; i < n_threads; i++) {
    // Crea una estructura con la información necesaria para el hilo
    struct thread_info *info = malloc(sizeof(struct thread_info));
    info->sockfd = sockfd;
    info->image_name = image;
    info->n_ciclos = n_ciclos;

    // Crea el hilo
    pthread_create(&threads[i], NULL, send_images, info);
    printf("Thread creado\n");
  }

  // Espera a que los hilos terminen
  for (int j = 0; j < n_threads; j++) {
    pthread_join(threads[j], NULL);
  }
  printf("Trabajos finalizados, cerrando cliente\n");
  wait_acknowledge_from_socket(sockfd, '#');
  close_socket(sockfd);
}

int main(int argc, char *argv[]) {
  int sockfd;
  struct sockaddr_in serv_addr;
  int opt;
  char *ip, *port, *image;
  int n_threads, n_ciclos;

  // Procesa los argumentos de la línea de comandos
  while ((opt = getopt(argc, argv, "a:p:i:t:c:")) != -1) {
    switch (opt) {
    case 'a':
      ip = optarg;
      break;
    case 'p':
      port = optarg;
      break;
    case 'i':
      image = optarg;
      break;
    case 't':
      n_threads = atoi(optarg);
      break;
    case 'c':
      n_ciclos = atoi(optarg);
      break;
    default:
      printf("Uso: %s -a <ip> -p <puerto> -i <imagen> -t <n_threads> -c "
             "<n_ciclos>\n",
             argv[0]);
      exit(1);
    }
  }

  // Comprueba que se hayan proporcionado los argumentos necesarios
  if (ip == NULL || port == NULL || image == NULL || n_threads <= 0 ||
      n_ciclos <= 0) {
    printf("Uso: %s -a <ip> -p <puerto> -i <imagen> -t <n_threads> -c "
           "<n_ciclos>\n",
           argv[0]);
    exit(1);
  }

  sockfd = create_socket(ip, port, &serv_addr);
  connect_socket(sockfd, serv_addr);
  create_threads(sockfd, image, n_threads, n_ciclos);

  return 0;
}

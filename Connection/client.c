#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>

#define CHUNK_SIZE 1024

struct thread_info {
    int sockfd;
    struct sockaddr_in serv_addr;
    char *image_name;
    int n_ciclos;
};

void *send_images_thread(void *arg) {
    struct thread_info *info = (struct thread_info*) arg;
    int sockfd = info->sockfd;
    struct sockaddr_in serv_addr = info->serv_addr;
    char *image_name = info->image_name;
    int n_ciclos = info->n_ciclos;

    for (int i = 0; i < n_ciclos; i++) {
        send_image(sockfd, serv_addr, image_name);
    }

    return NULL;
}

void send_image(int sockfd, struct sockaddr_in serv_addr, char *image_name) {

    FILE *fp;
    char buffer[CHUNK_SIZE];
    int n;
    
    // Abre el archivo de imagen
    fp = fopen(image_name, "rb");
    if (fp == NULL) {
        printf("Error al abrir el archivo de imagen\n");
        exit(1);
    }
    //While hasta que se conecte al server
    while(1){
        // Conecta con el servidor
        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            printf("Error al conectarse con el servidor\n");
        }
        else{break;}
    }
    
    // Lee el archivo de imagen y envía los chunks al servidor
    while ((n = fread(buffer, 1, CHUNK_SIZE, fp)) > 0) {
        if (send(sockfd, buffer, n, 0) < 0) {
            printf("Error al enviar los datos al servidor\n");
            exit(1);
        }
    }

    // Cierra el archivo
    fclose(fp);
}

int main(int argc, char *argv[]) {
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
                printf("Uso: %s -a <ip> -p <puerto> -i <imagen> -t <n_threads> -c <n_ciclos>\n", argv[0]);
                exit(1);
        }
    }

    // Comprueba que se hayan proporcionado los argumentos necesarios
    if (ip == NULL || port == NULL || image == NULL || n_threads <= 0 || n_ciclos <= 0) {
        printf("Uso: %s -a <ip> -p <puerto> -i <imagen> -t <n_threads> -c <n_ciclos>\n", argv[0]);
        exit(1);
    }

    // Crea un socket
    int sockfd = create_socket(ip, port);

    // Configura la dirección del servidor
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(port));
    inet_pton(AF_INET, ip, &serv_addr.sin_addr);

    // Crea una estructura con la información necesaria para los hilos
    struct thread_info info = {
        .sockfd = sockfd,
        .serv_addr = serv_addr,
        .image_name = image,
        .n_ciclos = n_ciclos
    };

    // Crea los hilos y espera a que terminen
    send_images_multithreaded(n_threads, &info);

    // Cierra el socket
    close(sockfd);

    return 0;
}
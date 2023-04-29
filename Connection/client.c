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

struct thread_info
{
    int sockfd;
    struct sockaddr_in serv_addr;
    char *image_name;
    char *port;
    char *ip;
    int n_ciclos;
};

int create_socket(char *ip, char *port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Error al crear el socket\n");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(port));
    inet_pton(AF_INET, ip, &serv_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Error al conectarse con el servidor\n");
        exit(1);
    }

    return sockfd;
}

void send_image(int sockfd, struct sockaddr_in serv_addr, char *image_name, char *port, char *ip)
{
    FILE *fp;
    char buffer[CHUNK_SIZE];
    int n;

    // Crea un socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Error al crear el socket\n");
        return;
    }

    // Configura la dirección del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(port));
    inet_pton(AF_INET, ip, &serv_addr.sin_addr);

    // Abre el archivo de imagen
    fp = fopen(image_name, "rb");
    if (fp == NULL)
    {
        printf("Error al abrir el archivo de imagen\n");
        exit(1);
    }
    // While hasta que se conecte al server
    while (1)
    {
        int timeout_seconds = 1;
        // Attempt to connect to server
        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0)
        {
            break;
        }

        // If connection failed, wait before retrying
        printf("Connection attempt failed, retrying in %d seconds...\n", timeout_seconds);
        sleep(timeout_seconds);
    }

    // Lee el archivo de imagen y envía los chunks al servidor
    while ((n = fread(buffer, 1, CHUNK_SIZE, fp)) > 0)
    {
        if (send(sockfd, buffer, n, 0) < 0)
        { 
            printf("Error al enviar los datos al servidor\n");
            exit(1);
        }
    }
    char *stop = "$";
    if (send(sockfd, stop, sizeof(stop), 0) < 0)
    {
        printf("Error al enviar los datos al servidor\n");
        exit(1);
    }

    // Cierra el archivo
    fclose(fp);
}

void *send_images(void *arg)
{
    struct thread_info *info = (struct thread_info *)arg;
    for (int i = 0; i < info->n_ciclos; i++)
    {
        send_image(info->sockfd, info->serv_addr, info->image_name, info->port, info->ip);
    }
    return NULL;
}

void create_threads(int sockfd, struct sockaddr_in serv_addr, char *ip, char *port, char *image, int n_threads, int n_ciclos)
{
    int total_ciclos = 0; // declaración y asignación del contador total_ciclos
    // Crea los hilos
    pthread_t threads[n_threads];
    for (int i = 0; i < n_threads; i++)
    {

        // Crea una estructura con la información necesaria para el hilo
        struct thread_info *info = malloc(sizeof(struct thread_info));
        info->sockfd = sockfd;
        info->serv_addr = serv_addr;
        info->image_name = image;
        info->port = port;
        info->ip = ip;
        info->n_ciclos = n_ciclos;

        // Crea el hilo
        pthread_create(&threads[i], NULL, send_images, info);
        printf("Thread creado\n");
    }

    // Espera a que los hilos terminen
    for (int j = 0; j < n_threads; j++)
    {
        pthread_join(threads[j], NULL);
    }

}

int main(int argc, char *argv[])
{

    int sockfd;
    struct sockaddr_in serv_addr;
    int opt;
    char *ip, *port, *image;
    int n_threads, n_ciclos;

    // Procesa los argumentos de la línea de comandos
    while ((opt = getopt(argc, argv, "a:p:i:t:c:")) != -1)
    {
        switch (opt)
        {
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
    if (ip == NULL || port == NULL || image == NULL || n_threads <= 0 || n_ciclos <= 0)
    {
        printf("Uso: %s -a <ip> -p <puerto> -i <imagen> -t <n_threads> -c <n_ciclos>\n", argv[0]);
        exit(1);
    }

    create_threads(sockfd, serv_addr, ip, port, image, n_threads, n_ciclos);

    return 0;
}

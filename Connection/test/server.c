#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define CHUNK_SIZE 1024
#define IMAGE_NAME "imagenrecibida.jpg"

int main() {
    FILE *fp;
    char buffer[CHUNK_SIZE];
    int sockfd, newsockfd, n;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    // Crea un socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Error al crear el socket\n");
        return 1;
    }

    // Configura la dirección del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // Asocia el socket con la dirección del servidor
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Error al asociar el socket con la dirección del servidor\n");
        return 1;
    }

    // Espera por conexiones entrantes
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0) {
        printf("Error al aceptar la conexión entrante\n");
        return 1;
    }

    // Crea un archivo para guardar la imagen recibida
    fp = fopen(IMAGE_NAME, "wb");
    if (fp == NULL) {
        printf("Error al crear el archivo de imagen\n");
        return 1;
    }

    // Recibe los chunks de la imagen y escribe en el archivo de imagen
    while ((n = recv(newsockfd, buffer, CHUNK_SIZE, 0)) > 0) {
        if (fwrite(buffer, 1, n, fp) != n) {
            printf("Error al escribir los datos en el archivo de imagen\n");
            return 1;
        }
    }

    // Cierra el archivo y el socket
    fclose(fp);
    close(newsockfd);
    close(sockfd);

    return 0;
}

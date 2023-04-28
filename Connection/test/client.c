#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define CHUNK_SIZE 1024

int main() {
    FILE *fp;
    char buffer[CHUNK_SIZE];
    int sockfd, n;
    struct sockaddr_in serv_addr;

    // Abre el archivo de imagen
    fp = fopen("imagen.jpg", "rb");
    if (fp == NULL) {
        printf("Error al abrir el archivo de imagen\n");
        return 1;
    }

    // Crea un socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Error al crear el socket\n");
        return 1;
    }

    // Configura la dirección del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // Conecta con el servidor
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Error al conectarse con el servidor\n");
        return 1;
    }

    // Lee el archivo de imagen y envía los chunks al servidor
    while ((n = fread(buffer, 1, CHUNK_SIZE, fp)) > 0) {
        if (send(sockfd, buffer, n, 0) < 0) {
            printf("Error al enviar los datos al servidor\n");
            return 1;
        }
    }

    // Cierra el archivo y el socket
    fclose(fp);
    close(sockfd);

    return 0;
}

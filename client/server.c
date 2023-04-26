#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    char message[1024];

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }

    if (listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    fd_set reads, temps;
    int fd_max, fd_num, str_len;
    struct timeval timeout;

    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    fd_max = serv_sock;

    while (1) {
        temps = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        if ((fd_num = select(fd_max + 1, &temps, 0, 0, &timeout)) == -1) {
            break;
        }

        if (fd_num == 0) {
            continue;
        }

        for (int i = 0; i < fd_max + 1; i++) {
            if (FD_ISSET(i, &temps)) {
                if (i == serv_sock) {
                    clnt_addr_size = sizeof(clnt_addr);
                    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
                    if (clnt_sock == -1) {
                        error_handling("accept() error");
                    }

                    FD_SET(clnt_sock, &reads);
                    if (clnt_sock > fd_max) {
                        fd_max = clnt_sock;
                    }
                } else {
                    str_len = read(i, message, sizeof(message) - 1);
                    if (str_len == 0) {
                        FD_CLR(i, &reads);
                        close(i);
                    } else {
                        message[str_len] = '\0';
                        printf("Received message: %s\n", message);
                    }
                }
            }
        }
    }

    close(serv_sock);
    return 0;
}

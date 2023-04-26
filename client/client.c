#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

typedef struct {
    char *ip;
    int port;
    char *message;
    int cycles;
} ThreadArgs;

void *client_thread(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int sock;
    struct sockaddr_in serv_addr;

    for (int i = 0; i < args->cycles; i++) {
        sock = socket(PF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            error_handling("socket() error");
        }

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(args->ip);
        serv_addr.sin_port = htons(args->port);

        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
            error_handling("connect() error");
        }

        write(sock, args->message, strlen(args->message) + 1);

        close(sock);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: %s <IP> <port> <message> <N-threads> <N-cycles>\n", argv[0]);
        exit(1);
    }

    int n_threads = atoi(argv[4]);
    int n_cycles = atoi(argv[5]);
    pthread_t *threads = malloc(sizeof(pthread_t) * n_threads);

    ThreadArgs args;
    args.ip = argv[1];
    args.port = atoi(argv[2]);
    args.message = argv[3];
    args.cycles = n_cycles;

    for (int i = 0; i < n_threads; i++) {
        if (pthread_create(&threads[i], NULL, client_thread, &args) != 0) {
            error_handling("pthread_create() error");
        }
    }

    for (int i = 0; i < n_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            error_handling("pthread_join() error");
        }
    }

    free(threads);

    return 0;
}

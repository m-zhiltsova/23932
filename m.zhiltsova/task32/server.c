#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <signal.h>

#define SOCKET_PATH "unix_socket"
#define BUFFER_SIZE 256

int server_socket;
pthread_mutex_t mutex;
int active_clients = 0;

void handle_sigint(int sig) {
    printf("Сервер завершает работу...\n");
    unlink(SOCKET_PATH);
    exit(0);
}

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

void* handle_client(void* arg) {
    int client_socket = *(int*)arg;
    free(arg);
    char buffer[BUFFER_SIZE];

    pthread_mutex_lock(&mutex);
    active_clients++;
    pthread_mutex_unlock(&mutex);

    while (1) {
        ssize_t n = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (n <= 0) {
            break;
        }

        buffer[n] = '\0';
        to_uppercase(buffer);
        printf("Получено от клиента: %s\n", buffer);
    }

    close(client_socket);

    pthread_mutex_lock(&mutex);
    active_clients--;
    printf("Клиент отключен. Активные клиенты: %d\n", active_clients);

    if (active_clients == 0) {
        printf("Все клиенты отключены. Сервер завершает работу.\n");
        handle_sigint(0);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    struct sockaddr_un addr;

    signal(SIGINT, handle_sigint);
    pthread_mutex_init(&mutex, NULL);

    if ((server_socket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    unlink(SOCKET_PATH);

    if (bind(server_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_socket, 5) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Сервер запущен. Ожидание подключений...\n");

    while (1) {
        int* client_socket = malloc(sizeof(int));
        struct sockaddr_un client_addr;
        socklen_t client_len = sizeof(client_addr);

        if ((*client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("accept");
            free(client_socket);
            continue;
        }

        printf("Клиент подключен.\n");

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, client_socket) != 0) {
            perror("pthread_create");
            free(client_socket);
        }

        pthread_detach(tid);
    }

    close(server_socket);
    pthread_mutex_destroy(&mutex);
    unlink(SOCKET_PATH);
    return 0;
}


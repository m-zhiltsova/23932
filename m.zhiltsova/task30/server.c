#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "unix_socket"
#define BUFFER_SIZE 256

int main() {
    int server_sock, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    if ((server_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 1) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен, ожидаем подключения клиента...\n");

    if ((client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Клиент подключен.\n");

    ssize_t bytes_read;
    while ((bytes_read = read(client_sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; 

        for (int i = 0; i < bytes_read; i++) {
            buffer[i] = toupper((unsigned char)buffer[i]);
        }

        printf("Получено от клиента: %s\n", buffer);
    }

    close(client_sock);
    close(server_sock);
    unlink(SOCKET_PATH);
    return 0;
}


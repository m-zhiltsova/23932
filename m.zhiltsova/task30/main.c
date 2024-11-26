#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>

#define SOCKET_PATH "/tmp/server"
#define BUFFER_SIZE 1024

int main() {
    int sockfd, client_sockfd;
    char buf[BUFFER_SIZE];

    // Создание сокета
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Ошибка создания сокета");
        exit(1);
    }

    // Удаляем существующий сокет, если он есть
    unlink(SOCKET_PATH);

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Привязываем сокет к адресу
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Ошибка привязки сокета");
        exit(1);
    }

    listen(sockfd, 5);  // Ожидаем подключения клиента

    while (1) {
        client_sockfd = accept(sockfd, NULL, NULL);
        if (client_sockfd < 0) {
            perror("Ошибка подключения клиента");
            continue;
        }

        // Принимаем данные от клиента
        while (fgets(buf, sizeof(buf), fdopen(client_sockfd, "r")) != NULL) {
            printf("Получено: %s", buf);
            buf[strlen(buf) - 1] = toupper(buf[strlen(buf) - 1]);  // Переводим последний символ в верхний регистр
            // Отправляем обратно клиенту
            write(client_sockfd, buf, strlen(buf));
        }

        close(client_sockfd);  // Закрываем соединение с клиентом
    }

    close(sockfd);
    return 0;
}

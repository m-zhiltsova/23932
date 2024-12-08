#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>

#define SOCKET_PATH "unix_socket"
#define BUFFER_SIZE 256

volatile sig_atomic_t active_clients = 0;

void handle_sigint(int sig) {
    printf("Received SIGINT, shutting down server...\n");
    exit(0);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];

    signal(SIGINT, handle_sigint);

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    fd_set read_fds;
    int max_fd;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);
        max_fd = server_socket;

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(server_socket, &read_fds)) {
            client_len = sizeof(client_addr);
            client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
            if (client_socket < 0) {
                perror("accept");
                continue;
            }

            active_clients++;

            if (fork() == 0) {
                close(server_socket);
                int n;
                while ((n = read(client_socket, buffer, sizeof(buffer))) > 0) {
                    buffer[n] = '\0';
                    for (int i = 0; i < n; i++) {
                        buffer[i] = toupper(buffer[i]);
                    }

                    printf("Received: %s", buffer);
                }
                close(client_socket);
                active_clients--;

                if (active_clients == 0) {
                    printf("All clients disconnected, shutting down server...\n");
                    exit(0);
                }
                exit(0);
            }
            close(client_socket);
        }
    }

    unlink(SOCKET_PATH);
    return 0;
}

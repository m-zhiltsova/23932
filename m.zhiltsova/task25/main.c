#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 256

int main() {
    int pipefd[2];
    pid_t cpid;
    char buffer[BUFFER_SIZE];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {
        close(pipefd[1]);

        ssize_t bytesRead;
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            for (ssize_t i = 0; i < bytesRead; i++) {
                buffer[i] = toupper((unsigned char)buffer[i]);
            }
            write(STDOUT_FILENO, buffer, bytesRead);
        }

        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    } else {
        close(pipefd[0]);

        const char *text = "Some text here \n";
        size_t textLength = strlen(text);

        write(pipefd[1], text, textLength);
        close(pipefd[1]); 

        wait(NULL);
        exit(EXIT_SUCCESS);
    }
}


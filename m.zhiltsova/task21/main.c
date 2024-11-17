#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t signal_count = 0;

void handle_sigint(int sig) {
    signal_count++;
    printf("\a");
    fflush(stdout);
}

void handle_sigquit(int sig) {
    printf("\nСигнал SIGINT был получен %d раз.\n", signal_count);
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);

    printf("Нажмите Ctrl+C для посылки сигнала SIGINT.\n");
    printf("Нажмите Ctrl+\\ для посылки сигнала SIGQUIT и завершения программы.\n");

    while (1) {
        pause();
    }

    return 0;
}

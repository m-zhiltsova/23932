#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Using: %s <command> [arguments...]\n", argv[0]);
        return 1;
    }

    pid_t pid = fork(); //
    
    if (pid < 0) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        execvp(argv[1], &argv[1]);
        
        perror("execvp");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Ending code: %d\n", WEXITSTATUS(status));
        } else {
            printf("Subprocess have finished.\n");
        }
    }

    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
  pid_t child_pid = fork();

  if (child_pid == 0) {
    // subprocess code
    execlp("cat", "cat", "file.txt", NULL);
    perror("execlp failed");
    exit(1);
  } else if (child_pid > 0) {
    // parent process code
    printf("Parent process started to work\n");
    wait(NULL);
    printf("Parent process ended to work after finishing of subprocess.\n");
  } else {
    perror("fork failed");
    exit(1);
  }

  return 0;
}


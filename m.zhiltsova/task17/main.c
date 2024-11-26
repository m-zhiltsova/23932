#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define MAX_LENGTH 40
#define ERASE 127   // Backspace
#define KILL 21     // Ctrl+C
#define CTRL_W 23   // Ctrl+W
#define CTRL_D 4    // Ctrl+D
#define CTRL_G 7

struct termios original_termios;

void restore_terminal_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

void configure_terminal_mode() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(restore_terminal_mode);

    new_termios = original_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void remove_last_word(char *line, int *current_length) {
    if (*current_length == 0) return;
    while (*current_length > 0 && line[*current_length - 1] == ' ') (*current_length)--;
    while (*current_length > 0 && line[*current_length - 1] != ' ') (*current_length)--;
    line[*current_length] = '\0';
}

void handle_user_input() {
    char buffer[MAX_LENGTH + 1] = {0};
    int current_length = 0;
    char input_char;

    while (1) {
        read(STDIN_FILENO, &input_char, 1);

        if (input_char == CTRL_D && current_length == 0) {
            break;
        } else if (input_char == ERASE) {
            if (current_length > 0) {
                current_length--;
                printf("\b \b");
            }
        } else if (input_char == KILL) {
            while (current_length > 0) {
                printf("\b \b");
                current_length--;
            }
            buffer[0] = '\0';
        } else if (input_char == CTRL_W) {
            remove_last_word(buffer, &current_length);
            printf("\r\033[K");
            printf("%s", buffer);
            fflush(stdout);
        } else if (input_char >= 32 && input_char <= 126) {
            if (current_length < MAX_LENGTH) {
                buffer[current_length++] = input_char;
                buffer[current_length] = '\0';
                putchar(input_char);
            } else {
                putchar('\n');
                buffer[0] = input_char;
                current_length = 1;
                buffer[current_length] = '\0';
                putchar(input_char);
            }
        } else {
            putchar(CTRL_G);
        }
        
        fflush(stdout);
    }
}

int main() {
    configure_terminal_mode();
    printf("Start typing (press CTRL-D at the beginning of the line to exit)\n");
    handle_user_input();
    printf("\nProgram finished working.\n");
    return 0;
}


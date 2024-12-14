#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <cstring>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: ./program command1 \"|\" command2\n";
        return 1;
    }

    char* first_command[argc];
    char* second_command[argc];

    int i;
    for (i = 1; strcmp(argv[i], "|") != 0; i++) {
        first_command[i - 1] = argv[i];
    }
    first_command[i - 1] = NULL;

    int j = 0;
    while (++i < argc) {
        second_command[j++] = argv[i];
    }
    second_command[j] = NULL;

    int redirect[2];
    if (pipe(redirect) == -1) {
        perror("pipe failed");
        return 1;
    }

    int pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        close(redirect[0]);
        dup2(redirect[1], STDOUT_FILENO); 
        close(redirect[1]); 
        execvp(first_command[0], first_command);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else {
        close(redirect[1]);
        dup2(redirect[0], STDIN_FILENO);
        close(redirect[0]);
        execvp(second_command[0], second_command);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}

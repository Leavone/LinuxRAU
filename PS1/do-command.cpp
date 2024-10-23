#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <chrono>    
#include <cstring>  

void do_command(char** argv) {
    auto start = std::chrono::high_resolution_clock::now();

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Couldn't fork";
        return;
    }

    if (pid == 0) {
        if (execvp(argv[0], argv) == -1) {
            std::cerr << "Couln't execute";
            exit(EXIT_FAILURE);
        }
    } else {
	int status;
	wait(&status);
      
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

      
       if (WIFEXITED(status)) {
            std::cout << "Success, Status: " << WEXITSTATUS(status) << '\n';
        } else if (WIFSIGNALED(status)) {
            std::cout << "Fail, Status:  " << WTERMSIG(status) << '\n';
        } else {
            std::cout << "Error" << '\n';
        }

        std::cout << "Executed in: " << duration.count() << '\n';
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "No arguments passed";
        return 1;
    }

    char** command_argv = new char*[argc];
    for (int i = 1; i < argc; ++i) {
        command_argv[i - 1] = argv[i];
    }
    command_argv[argc - 1] = nullptr;
    do_command(command_argv);
    delete[] command_argv;

    return 0;
}

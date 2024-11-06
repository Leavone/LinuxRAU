#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
	if(argc != 3){
		std::cerr<<"Not enough args\n";
		return 1;
	}

	int child = fork();

	if (child == 0){
		char* const args[2] = {argv[1], nullptr};
		return execvp(argv[1], args);
	}
	int wstatus;
	wait(&wstatus);
	if(WIFEXITED(wstatus)){
		if(WEXITSTATUS(wstatus) == 0){
			char* const args[2] = {argv[2], nullptr};
			execvp(argv[2], args);
			return 0;
		}
	} else {
		std::cerr<<"Error\n";
		return 1;
	}
	
}

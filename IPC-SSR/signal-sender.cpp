#include <iostream>
#include <signal.h>

int main(int argc, char* argv[]){
	int signum = atoi(argv[1]);
	int dest_pid = atoi(argv[2]);
	if(kill(dest_pid, signum) != 0){
		std::cerr<<"Couldn't send signal\n";
		return 1;
	}
}

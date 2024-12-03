#include <iostream>
#include <signal.h>

void handler(int signum){
	static int count = 0;
	if(signum == SIGUSR1)
		return;
	if(signum == SIGUSR2){
		count++;
		return;
	}
	if(signum == SIGINT){
		std::cout<<"SIGUSR2 signal has been sent " << count <<" times\nExiting program.\n";
		exit(0);
	}
}

int main(){
	signal(SIGUSR1, handler);
	signal(SIGUSR2, handler);
	signal(SIGINT, handler);

	while(true){
		std::cout<<"I am still alive!\n";
		sleep(5);
	}
}

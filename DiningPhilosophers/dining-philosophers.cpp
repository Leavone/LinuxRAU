#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <string>
#include <sys/wait.h>

sem_t *sticks[5];

void philosophy(int id){
	int left = id;
	int right = (id + 1)%5;

	if(id == 0){
		int t = left;
		left = right;
		right = t;
	}

	while(true){
		int time = rand() % 4 + 1;
		std::cout<<"Thinking... (Philosopher number "<<id<<")\n";
		sleep(time);

		sem_wait(sticks[left]);
		sem_wait(sticks[right]);

		int eat_time = rand() % 4 + 1;
		std::cout<<"Eating... (Philosopher number "<<id<<")\n";
		sleep(eat_time);

		sem_post(sticks[left]);
		sem_post(sticks[right]);
	}
	std::cout<<id<<'\n';
}

int main(){
	pid_t pids[5];
	for(int i = 0; i < 5; i++){
		std::string name = "stick_"+std::to_string(i);
		sem_unlink(name.c_str());
		sticks[i] = sem_open(name.c_str(), O_CREAT, 0644, 1);
	}
	for (int i = 0; i < 5; i++) {
        	pids[i] = fork();
        	if (pids[i] == 0) {
            		philosophy(i);
            		exit(0);
        	}
   	}		
	wait(NULL);
	for(int i = 0; i < 5; i++){
		sem_close(sticks[i]);
	}	
}


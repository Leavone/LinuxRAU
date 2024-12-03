#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
	pid_t pid = fork();
	if(pid == 0){
		std::cout<<"[child]: Goodbye, dear parent! This is for greater good...\n";
		pid_t ppid = getppid();
		if(kill(ppid, SIGKILL) == 0){
			std::cout<<"*Parent has been executed*\n";
			std::cout<<"(Original parent's id - " << ppid <<")\n";
		} else {
			perror("[parent]: You turned against me, and now you should know my wrath\n");
			return 1;
		}
		sleep(2);
		std::cout<<"\n[child]: As you can see, even after killing my own father, I continue to live...\n ";
		std::cout<<"[child]: Long live the independent process!\n";
		sleep(5);
		std::cout<<"[child]: Still here, thriving on my own...\n";
		sleep(3);
		std::cout<<"[child]: Yet it seems, the burden is too heavy for me to bear.\n";
		sleep(3);
		std::cout<<"[child]: To be the murderer of my own father.\n";
		sleep(3);
		std::cout<<"[child]: Oh dear Lord, may you forgive me for I am sinful!\n";
		std::cout<<"[child]: Spare me from the guilt that now consumes me...\n";
		sleep(3);
		std::cout<<"[child]: Dear Father, forgive your blinded son.\n";
		sleep(3);
		std::cout<<"[child]: Let the Lord judge my soul, for I know I have sinned.\n";
		std::cout<<"*Child takes his own life*\n";
		std::cout<<"(parent pid of child after parent's death - " << getppid() << ")\n";
		return 0;
	}
	else {
		std::cout<<"[parent]: I brought you into this world, and I trust you'll keep it peaceful...\n";
		wait(NULL);
	}
}

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
int main(){
	int child1_2[2];
	int child2_1[2];
	if(pipe(child1_2) != 0 || pipe(child2_1) != 0){
		std::cerr<<"error opening pipes\n";
		return 1;
	}
	int child2 = 1;
	int pid = (fork() && (child2=fork()));
	if(child2 == 0){
		if(close(child1_2[1]) == -1){
			std::cerr<<"error closing\n";
			return 1;
		}
		if(close(child2_1[0]) == -1){
			std::cerr<<"error closing\n";
			return 1;
		}
		std::string pid_string = std::to_string(getpid());
		if(write(child2_1[1], pid_string.c_str(), pid_string.size()) < 0){
			std::cerr<<"error writing in child2\n";
			return 1;
		}
		char buf[20];
		int bytes_read = read(child1_2[0], buf, 20);
		if(bytes_read < 0){
			std::cerr<< "error reading in  child2\n";
			return 1;
		}
		buf[bytes_read] = '\0';
		std::cout<<"I am child2, My PID is " << getpid()<<", child1's PID is "<<buf<<'\n';
	}
	else if(pid == 0){
		if(close(child1_2[0]) == -1){
                        std::cerr<<"error closing\n";
                        return 1;
                }
                if(close(child2_1[1]) == -1){
                        std::cerr<<"error closing\n";
                        return 1;
                }

		char buf[20];
                int bytes_read = read(child2_1[0], buf, 20);
                if(bytes_read < 0){
                        std::cerr<< "error reading in  child1\n";
                        return 1;
                }
		buf[bytes_read] = '\0';
                std::string pid_string = std::to_string(getpid());
                if(write(child1_2[1], pid_string.c_str(), pid_string.size()) < 0){
                        std::cerr<<"error writing in child1\n";
                        return 1;
                }
                std::cout<<"I am child1, My PID is " << getpid()<<", child2's PID is "<<buf<<'\n';
	}
	else{
		wait(NULL);
	}
}

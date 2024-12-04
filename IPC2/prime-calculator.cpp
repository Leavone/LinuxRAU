#include <iostream>
#include <unistd.h>
#include <string>
#include <signal.h>
#include <sys/wait.h>

bool is_prime(int n){
	if(n <= 1)
		return false;
	for(int i = 2; i*i <= n; i++){
		if(n%i==0)
			return false;
	}
	return true;
}

int nth_prime(int n){
	int i = 2;
	while(n>0){
		if(is_prime(i))
			n--;
		i++;
	}
	return i-1;
}

int main(){
	int p_to_c[2];
	int c_to_p[2];
	if(pipe(p_to_c) == -1 || pipe(c_to_p) == -1){
		perror("pipe");
		return 1;
	}

	int pid = fork();
	if(pid < 0){
		perror("fork");
		return 1;
	}

	if(pid == 0){
		close(p_to_c[1]);
		close(c_to_p[0]);
		while(true){
			char buf[100];
			if(read(p_to_c[0], buf, 100) < 0){
				perror("error reading in child\n");
				return 1;
			}
			if(std::string(buf) == "exit"){
				close(p_to_c[0]);
				close(c_to_p[1]);
				std::cout<<"CHILD TERMINATED\n";
				return 0;
			}
			int n = atoi(buf);
			std::cout<<"[Child] Calculating "<<n<<"-th prime number...\n";
			int prime_num = nth_prime(n);
			std::cout<<"[Child] Sending calculation result of prime("<<n<<")...\n";
			if(write(c_to_p[1], std::to_string(prime_num).c_str(), 100) < 0){
				perror("error writin in child\n");
				return 1;
			}
		}
	}
	else{
		close(p_to_c[0]);
		close(c_to_p[1]);
		while(true){
			std::string inp;
			std::cout<<"[Parent] Please enter the number: ";
			std::cin>>inp;
			if(inp == "exit"){
				write(p_to_c[1], "exit\0", 5);
				close(p_to_c[1]);
				close(c_to_p[0]);
				wait(NULL);
				std::cout<<"PARENT TERMINATED\n";
				return 0;
			}
			std::cout<<"[Parent] Sending "<<inp<<" to the child process...\n";
			int n = atoi(inp.c_str());
			if(write(p_to_c[1], std::to_string(n).c_str(), 100) < 0){
				perror("error writing in parent\n");
				return 1;
			}
			std::cout<<"[Parent] Waiting for the response from the child process...\n";
			char buf[100];
			if(read(c_to_p[0], buf, 100) < 0){
				perror("error reading in parent\n");
				return 1;
			}
			std::cout<<"[Parent] Received calculation result of prime("<<n<<") = "<<buf<<"...\n";
		}
	}


}

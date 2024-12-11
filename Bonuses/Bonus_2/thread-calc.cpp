#include <string>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

struct args{
        int a;
        int b;
        std::string file_name;
        std::string command;
};

void* calc(void* arg){
	struct args* arguments = reinterpret_cast<struct args *>(arg);
	
	int fd = open(arguments->file_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(fd == -1){
		std::cerr<<"error opening file\n";
		exit(1);
	}
	int result = 0;
	if(arguments->command == "s"){
		result = arguments->a + arguments->b;
	} else if(arguments->command == "m"){
                result = arguments->a * arguments->b;
        } else if(arguments->command == "ss"){
                result = (arguments->a*arguments->a) + (arguments->b*arguments->b);
        }
	std::string to_write = std::to_string(arguments->a) + " " +arguments->command + " " + std::to_string(arguments->b) + " = " + std::to_string(result);
	int s_size = to_write.size();
	if(write(fd, to_write.c_str(), s_size) == -1){
		std::cerr<<"error writing to file\n";
		close(fd);
		exit(1);
	}
	if(close(fd) == -1){
		std::cerr<<"error closing file\n";
		exit(1);
	}
	return nullptr;
}

int main(){
	int n;
	std::cin>>n;
	args* arguments = new args[n];
	pthread_t* thread_ids = new pthread_t[n];
	for(int i = 0; i < n; i++){
		int a, b;
		std::string command;
		std::cin>>a>>b>>command;
		arguments[i] = args{a, b, "out_"+std::to_string(i+1), command};
	}

	for(int i = 0; i < n; i++){
		int ret = pthread_create(&thread_ids[i], NULL, *calc, (void*)&arguments[i]);
		if (ret != 0){
			std::cerr<<"error creating thread\n";
			return 1;
		}
	}
	for(int i = 0; i < n; i++){
                int ret = pthread_join(thread_ids[i], NULL);
		if (ret != 0){
			std::cerr<<"error joining the thread\n";
			return 1;
		}
        }

}

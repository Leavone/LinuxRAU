#include <dirent.h>
#include <iostream>
#include <sys/stat.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void traverse_directory(std::string path){
	DIR* dir = opendir(path.c_str());
        if(!dir){
                std::cerr<<"Error: " << strerror(errno)<<'\n';
                exit(1);
        }
        struct dirent* entry;
        while((entry = readdir(dir)) != nullptr){

                std::string name = entry->d_name;
                if(name == "." || name == "..")
                        continue;

                std::string full_path = path + "/" + name;
                struct stat info;
                if(stat(full_path.c_str(), &info) == -1){
                        std::cerr<<"error reading stats";
                        exit(1);
                }

		if(S_ISDIR(info.st_mode)){
			traverse_directory(full_path);
		} else if (S_ISREG(info.st_mode)){
			int fd = open(full_path.c_str(), O_RDONLY, NULL);
			if(fd == -1){
				std::cerr<<"error opening file\n";
				exit(1);
			}
			char buf[1000];
			int bytes_read;
			do{
				bytes_read = read(fd, buf, 999);
				if(bytes_read < 0){
					std::cerr<<"error reading from file\n";
					exit(1);
				}
				buf[bytes_read] = '\0';
				std::cout<<buf;
			} while(bytes_read > 0);
			std::cout<<'\n';
			if(close(fd) != 0){
				std::cerr<<"error closing file\n";
				exit(1);
			}
		}
        }

}

int main(int argc, char* argv[]){
	if(argc == 1){
		std::cerr<<"not enough arguments\n";
		return 1;
	}
	std::string path = argv[1];
	traverse_directory(path);
}

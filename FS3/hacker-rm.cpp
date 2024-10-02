#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define BUFF_SIZE 4096

int main(int argc, char** argv){

	 if(argc == 1){
                perror("File destination has not been passed");
                return 1;
        }

	int fd = open(argv[1], O_RDWR);
	
	if (fd == -1){
		perror("Couldn't open the file");
		exit(EXIT_FAILURE);
	}

	char buffer[BUFF_SIZE] = {'\0'};
	int file_size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	while(file_size > BUFF_SIZE){
		write(fd, buffer, BUFF_SIZE);
		file_size -= BUFF_SIZE;
		std::cout<<file_size<<'\n';
	}
	write(fd, buffer, file_size);
	close(fd);
	unlink(argv[1]);
	return 0;
}

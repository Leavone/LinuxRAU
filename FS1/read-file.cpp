#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char **argv){
	char buffer[100];
	int fd = open(argv[1], O_RDONLY);
	if(fd == -1){
		std::cout<<"Couldn't read the file\n";
		exit(EXIT_FAILURE);
	}
	while(true){
		ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
		if(bytes_read == 0)
			break;
		buffer[bytes_read] = '\0';
		std::cout<<buffer;
	}
	close(fd);
	return 0;
}

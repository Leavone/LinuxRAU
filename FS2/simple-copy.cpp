#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv){
	int fd = open(argv[1], O_RDONLY);
	if(fd == -1){
		perror("Couldn't load the file");
		return 1;
	}
	int dest = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0664);
	if(dest == -1){
		perror("Couldn't load or create destination file");
		exit(EXIT_FAILURE);
	}

	char buffer[100];
	while(true){
		ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
		if(bytes_read == 0)
			break;
		write(dest, buffer, bytes_read);
	}
	close(fd);
	close(dest);
	return 0;
}


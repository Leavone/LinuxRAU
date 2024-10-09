#include <fcntl.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char** argv){
	if(argc != 2){
		perror("Arguments are passed wrong");
		exit(1);
	}

	int fd1 = open(argv[1], O_WRONLY | O_CREAT, 0644);
	if(fd1 < 0){
		perror("Error opening file");
		exit(1);
	}
	int fd2 = dup(fd1);
	if(fd2 < 0){
		perror("Error opening file");
		exit(1);
	}
	char line1[] = "first line\n";
	char line2[] = "second line";
	if(write(fd1, line1, sizeof(line1)) < 0){
		perror("Error writing into file");
		exit(1);
	}
	
	if(write(fd2, line2, sizeof(line2)) < 0){
                perror("Error writing into file");
                exit(1);
        }
	return 0;
}

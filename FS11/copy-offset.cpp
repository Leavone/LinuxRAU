#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int source_offset = 0;
    int dest_offset = 0;
    if (argc < 3) {
        perror("Not all arguments have been passed");
        return 1;
    }
    else if (argc == 4){
    	source_offset = atoi(argv[3]);
    }
    else if(argc > 4){
	source_offset = atoi(argv[3]);
    	dest_offset = atoi(argv[4]);
    }
    
    int source = open(argv[1], O_RDONLY);
    if (source == -1) {
        perror("Couldn't load the source file");
        return 1;
    }
    
    // Opening destination file
    int destination = open(argv[2], O_CREAT | O_RDWR, 0664);
    if (destination == -1) {
        perror("Couldn't load or create destination file");
        exit(EXIT_FAILURE);
    }
    
    char buffer[100];

    lseek(source, source_offset, SEEK_SET);
    lseek(destination, dest_offset, SEEK_SET);
    
    // Reading from source until bytes_read is 0 and writing into destination
    while (true) {
        ssize_t bytes_read = read(source, buffer, sizeof(buffer) - 1);
        if (bytes_read == 0)
            break;
        write(destination, buffer, bytes_read-1);
    }
    
    close(source);
    close(destination);
    
    return 0;
}

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE 4096

int main(int argc, char **argv) {

    // Return if no argument has been passed
    if (argc == 1) {
        perror("File destination has not been passed");
        return 1;
    }

    int fd = open(argv[1], O_RDWR);

    if (fd == -1) {
        perror("Couldn't open the file");
        exit(EXIT_FAILURE);
    }

    // Initialize null buffer of defined size
    char buffer[BUFF_SIZE] = {'\0'};

    // Get the number of bytes in the file (it sends cursor to the end)
    int file_size = lseek(fd, 0, SEEK_END);

    // Return cursor to the begining
    lseek(fd, 0, SEEK_SET);

    // Write buffer into the file
    while (file_size > BUFF_SIZE) {
        write(fd, buffer, BUFF_SIZE);
        file_size -= BUFF_SIZE;
        std::cout << file_size << '\n';
    }
    write(fd, buffer, file_size);

    close(fd);

    // Unlinking passed filename from the data
    unlink(argv[1]);
    return 0;
}

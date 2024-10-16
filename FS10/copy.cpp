#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {

    if (argc != 3) {
        perror("Not all arguments have been passed");
        return 1;
    }

    int source = open(argv[1], O_RDONLY);
    if (source == -1) {
        perror("Couldn't load the source file");
        return 1;
    }
    
    // Opening destination file
    int destination = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0664);
    if (destination == -1) {
        perror("Couldn't load or create destination file");
        exit(EXIT_FAILURE);
    }
    
    char buffer[100];
    
    // Reading from source until bytes_read is 0 and writing into destination
    int src_offset = 0;
    int data_count = 0;
    int hole_count = 0;
    while (true) {
	int data_start = lseek(source, src_offset, SEEK_DATA);
	if (data_start == -1)
		break;
	if (data_start > src_offset){
		hole_count += data_start - src_offset;

		if(lseek(destination, data_start - src_offset, SEEK_END) == -1){
			std::cerr<<"Couldn't write hole";
			close(source);
			close(destination);
			return 1;
		}
	}
	src_offset = data_start;

	int hole_start = lseek(source, src_offset, SEEK_HOLE);
	if (hole_start == -1){
		hole_start = lseek(source, 0, SEEK_END);
	}

	int curr_data_count = hole_start - src_offset;
	data_count += curr_data_count;
	lseek(source, src_offset, SEEK_SET);
	while(curr_data_count > 0){
                int len = sizeof(buffer) < curr_data_count ? sizeof(buffer) : curr_data_count;
                ssize_t bytes_read = read(source, buffer, len);

		if (bytes_read < 0){
			std::cerr<<"Error while reading";
			close(source);
			close(destination);
			return 1;
		}
		if (bytes_read == 0)
			break;
	        int bytes_written = write(destination, buffer, bytes_read);
		if (bytes_written < 0){
			std::cerr<<"Error while writing";
			close(source);
			close(destination);
			return 1;
		}
		curr_data_count -= bytes_written;
	}
	src_offset = hole_start;
    }
    
    std::cout<<"Successfully copied " << data_count + hole_count << " bytes (data: "<< data_count<<", hole: "<< hole_count<<")\n";
    close(source);
    close(destination);
    
    return 0;
}

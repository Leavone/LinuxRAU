#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>

const char* SHM_NAME = "/eratosphene";
const size_t SHM_SIZE = 1000000;

int main(){
	const size_t size = SHM_SIZE*sizeof(int);
	int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
	if(shm_fd == -1){
		perror("shm_open");
		return 1;
	}

	if(ftruncate(shm_fd, size) == -1){
		perror("ftruncate");
		return 1;
	}

	void* shm_ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	int* arr = (int*) shm_ptr;
	std::memset(arr, 0, size);

	munmap(shm_ptr, size);
	close(shm_fd);
}

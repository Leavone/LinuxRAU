#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>

const char* SHM_NAME = "/eratosphene";
const size_t SHM_SIZE = 1000000;

int main(){
	const size_t size = SHM_SIZE*sizeof(int);
	int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
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
	for(int i = 2; i*i < SHM_SIZE; i++){
		if(arr[i] != 1){
		for(int j = 2*i; j < SHM_SIZE; j+=i)
			arr[j] = 1;
		}
	}
	munmap(shm_ptr, size);
	close(shm_fd);
}

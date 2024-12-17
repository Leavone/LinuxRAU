#include "shared-array.h"

shared_array::shared_array(const std::string &name, size_t size)
	: name(name), size(size), data(nullptr), sem(nullptr){
		std::cout<<("/" + name + std::to_string(size))<<'\n';
		int shm_fd = shm_open(("/" + name + std::to_string(size)).c_str(), O_CREAT | O_RDWR, 0666);
		if(shm_fd == -1){
			perror("shm_open");
			exit(1);
		}

		if(ftruncate(shm_fd, size * sizeof(int)) == -1){
			perror("ftruncate");
			exit(1);
		}

		void* shm_ptr = mmap(nullptr, size*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
		if(shm_ptr == MAP_FAILED){
			perror("mmap");
			exit(1);
		}
		close(shm_fd);
		data = (int*)shm_ptr;
		sem = sem_open(("/" + name + std::to_string(size) + "_sem").c_str(), O_CREAT, 0666, 1);
		if(sem == SEM_FAILED){
			perror("sem_open");
			exit(1);
		}
}

shared_array::~shared_array(){
	if(data){
		munmap(data, size*sizeof(int));
		shm_unlink(("/" + name + std::to_string(size)).c_str());
	}
	if(sem){
		sem_close(sem);
		sem_unlink(("/" + name + std::to_string(size)+"_sem").c_str());
	}
}

int& shared_array::operator[](size_t i){
	if(i >= size){
		std::cerr<<"Out of range\n";
		exit(1);
	}

	sem_wait(sem);
	int& element = data[i];
	sem_post(sem);
	return element;
}

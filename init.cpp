#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

class Cell {
private:
	int cur_balance;
	int min_balance;
	int max_balance;
	bool is_frozen;

public:
	Cell() : cur_balance(0), min_balance(0), max_balance(1000000), is_frozen(false) {}
};

class Bank{
public:
	Bank(int N) {
		size = N;
		for(int i = 0; i < size; ++i) {
			cells[i] = Cell();
		}
	}
private:
	size_t size;
	Cell cells[];
};


int main(int argc, char* argv[]) {
  if(argc!=2){
  	perror("Wrong number of arguments");
	exit(1);
  }
  int N = atoi(argv[1]);
  const char *name = "/bank_shm";
  int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
  if(fd == -1){
  	perror("shm_open");
	exit(EXIT_FAILURE);
  }
  int shm_size = sizeof(Bank) + N * sizeof(Cell);
  ftruncate(fd, shm_size);
  void* ptr = mmap(0, shm_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
  if(ptr == MAP_FAILED){
  	perror("mmap");
	exit(EXIT_FAILURE);
  }

  Bank* bank_ptr = new (ptr) Bank(N);

  munmap(ptr, shm_size);
  close(fd);

  return 0;

}

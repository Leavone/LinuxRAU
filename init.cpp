#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <bank.h>

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

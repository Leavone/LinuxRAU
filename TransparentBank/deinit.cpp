#include <fcntl.h>
#include <sys/mman.h>

int main(){
	shm_unlink("/bank_shm");
	return 0;
}

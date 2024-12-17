#include <sys/mman.h>

const char* SHM_NAME = "/eratosphene";

int main(){
	shm_unlink(SHM_NAME);
}

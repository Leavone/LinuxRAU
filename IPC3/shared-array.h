#include <iostream>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cstring>
#include <unistd.h>

class shared_array{
public:
	shared_array(const std::string &name, size_t size);
	~shared_array();

	int &operator[](size_t i);

private:
	size_t size;
	std::string name;
	int* data;
	sem_t* sem;
};

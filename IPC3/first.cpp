#include <iostream>
#include <string>
#include <unistd.h>
#include "shared-array.h"

int main(){
	std::string name;
	size_t size;
	std::cin>>name>>size;

	shared_array array(name, size);

	while(true){
		for(int i = 0; i < size; i++){
			array[i]++;
		}
		sleep(1);
	}

}

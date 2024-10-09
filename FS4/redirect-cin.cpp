#include <fcntl.h>
#include <unistd.h>
#include <iostream>

void initialize(int argc, char** argv){
	
	if (argc == 1){
		perror("File has not been passed");
		exit(1);
	}

	close(0);
	int fd = open(argv[1], O_RDONLY);
	
	if (fd < 0){
		perror("Couldn't open passed file");
		exit(1);
	} 	
}

int main(int argc, char** argv){
	initialize(argc, argv);
	std::string input;
	std::cin >> input;

	std::string reversed;
	int len = input.length();
	for (int i = 0; i < len; i++){
		reversed.push_back(input[len - i - 1]);
	}

	std::cout << reversed << '\n';
	return 0;
}

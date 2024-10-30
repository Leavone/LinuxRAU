#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <chrono>    
#include <cstring>
#include <vector>
#include <string>
#include <sstream>
#include <fcntl.h>

int do_command(std::vector<char*> argv, bool silent, const std::string& output_file="", bool append=false) {
    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Couldn't fork\n";
        return -1;
    }

    if (pid == 0) {
	std::string cur_dir = get_current_dir_name();
	setenv("PATH", (cur_dir + ':' + getenv("PATH")).c_str(), 1);
	if(silent){
		std::string filename = std::to_string(getpid()) + ".log";
		int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if(fd == -1){
			std::cerr << "Error opening log file\n";
			exit(EXIT_FAILURE);
		}
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		close(fd);
	}
	if(!output_file.empty()){
		int fd = open(output_file.c_str(), O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
		if(fd == -1){
			std::cerr << "Error opening output log file\n";
			exit(EXIT_FAILURE);
		}
		dup2(fd, STDOUT_FILENO);
                dup2(fd, STDERR_FILENO);
                close(fd);
	}
	if (execvp(argv[0], argv.data()) == -1) {
            std::cerr << "Couln't execute\n";
            exit(EXIT_FAILURE);
        }
    } else {
	int status;
	wait(&status);
	return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }
}

std::vector<std::string> split(const std::string& command){
	std::istringstream iss(command);
	std::vector<std::string> output;
	std::string word;
	while(iss >> word){
		output.push_back(word);	
	}
	return output;
}

int main() {
    std::string input;
    while(true){
	bool silent = false;
    	std::cout<<"shell> ";
	std::getline(std::cin, input);
	if(input.empty())
		continue;	
	std::vector<std::string> arguments = split(input);
	if(arguments[0] == "exit")
		return 0;
	if(arguments[0] == "silent"){
		silent = true;
		arguments.erase(arguments.begin());
	}
	std::vector<char*> command_argv;
	bool append = false;
	bool redirect_output = false;
	int last_status = 0;
	bool execute_last = true;
	for(const std::string& argument : arguments){
		if(redirect_output){
			last_status = do_command(command_argv, silent, argument, append);
			redirect_output = false;
			append = false;
			command_argv.clear();
			continue;
		}
		if(argument == ";"){
			do_command(command_argv, silent);
			command_argv.clear();
		}
		else if(argument == ">"){
			redirect_output = true;
			continue;
		}
		else if(argument == ">>"){
			redirect_output = true;
			append = true;
			continue;
		}
		else if(argument == "||"){
			if(command_argv.empty() && last_status == 0)
                                break;
                        if(command_argv.empty() && last_status != 0)
                                continue;
			if(do_command(command_argv, silent) == 0){
				execute_last = false;
				break;
			}
			command_argv.clear();
		}
		else if(argument == "&&"){
			if(command_argv.empty() && last_status == 0)
				continue;
			if(command_argv.empty() && last_status != 0)
				break;
			if(do_command(command_argv, silent) != 0){
				execute_last = false;
				break;
			}
			command_argv.clear();
		}
		else {
			command_argv.push_back(const_cast<char*>(argument.c_str()));
		}
	}
	command_argv.push_back(nullptr);
	if(!command_argv.empty() && execute_last)
		do_command(command_argv, silent);
    }
    return 0;
    //for (int i = 1; i < argc; ++i) {
    //    command_argv[i - 1] = argv[i];
    //}
    //command_argv[argc - 1] = nullptr;
    //do_command(command_argv);
    //delete[] command_argv;

    //return 0;
}

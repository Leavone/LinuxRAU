#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "bank.h"

#define SHM_NAME "/bank_shm"

Bank* attach_bank() {
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if(fd == -1) {
        perror("shm_open");
        return nullptr;
    }

    off_t size = lseek(fd, 0, SEEK_END);
    if(size == -1) {
        perror("lseek");
        close(fd);
        return nullptr;
    }

    void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    
    if(ptr == MAP_FAILED) {
        perror("mmap");
        return nullptr;
    }

    return reinterpret_cast<Bank*>(ptr);
}

void print_help() {
    std::cout << "Available commands:\n"
              << "  balance <account> [current|min|max]\n"
              << "  freeze <account> [on|off]\n"
              << "  transfer <from> <to> <amount>\n"
              << "  credit_all <amount>\n"
              << "  debit_all <amount>\n"
              << "  set_min <account> <amount>\n"
              << "  set_max <account> <amount>\n"
              << "  help\n"
              << "  exit\n";
}

bool process_command(Bank* bank, const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;

    if(action == "balance") {
        int account;
        std::string type;
        if(!(iss >> account >> type)) {
            std::cerr << "Invalid arguments" << std::endl;
            return false;
        }
        bank->get_balance(account, type);
	return true;
    }
    else if(action == "transfer") {
        int from, to, amount;
        if(!(iss >> from >> to >> amount) || amount <= 0) {
            std::cerr << "Invalid arguments" << std::endl;
            return false;
        }

        if (bank->transfer(from, to, amount)) {
            std::cout << "Successfully transferred " << amount << " from account " << from << " to account " << to << ".\n";
        } else {
            std::cerr << "Transfer failed: check if accounts are valid, not frozen, and limits are respected.\n";
        } 
        return false;
    }
    else if(action == "freeze") {
        int account;
        std::string on_off;
        if(!(iss >> account >> on_off)) {
            std::cerr << "Invalid arguments" << std::endl;
            return false;
        }
        bank->freeze_account(account, on_off=="on");
        return true;
    }
    else if(action == "credit_all") {
        int amount;
        if(!(iss >> amount)) {
            std::cerr << "Invalid arguments" << std::endl;
            return false;
        }
        bank->credit_all(amount);
        return true;
    }
    else if(action == "debit_all") {
        int amount;
        if(!(iss >> amount)) {
            std::cerr << "Invalid arguments" << std::endl;
            return false;
        }
        bank->debit_all(amount);
        return true;
    }
    else if(action == "set_min") {
        int account, amount;
        if(!(iss >> account >> amount)) {
            std::cerr << "Invalid arguments" << std::endl;
            return false;
        }
        bank->set_min(account, amount);
        return true;
    }
    else if(action == "set_max") {
        int account, amount;
        if(!(iss >> account >> amount)) {
            std::cerr << "Invalid arguments" << std::endl;
            return false;
        }
        bank->set_max(account, amount);
        return true;
    }
    else if(action == "help") {
        print_help();
        return true;
    }
    else if(action == "exit") {
        return false;
    }
    else {
        std::cerr << "Unknown command" << std::endl;
        return false;
    }
}

int main() {
    Bank* bank = attach_bank();
    if(!bank) return EXIT_FAILURE;

    print_help();
    std::string command;
    while(std::cout << "> ", std::getline(std::cin, command)) {
        if(!process_command(bank, command)) {
            if(command == "exit") break;
        }
    }

    size_t size = sizeof(Bank) + bank->get_size() * sizeof(Cell);
    munmap(bank, size);
    return EXIT_SUCCESS;
}

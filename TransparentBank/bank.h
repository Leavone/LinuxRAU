#pragma once
#include <pthread.h>
#include <iostream>
#include <string>

// Represents a single bank account
class Cell {
private:
    int cur_balance;
    int min_balance;
    int max_balance;
    bool is_frozen;

public:
    Cell() : cur_balance(0), min_balance(0), max_balance(1000000), is_frozen(false) {}

    bool set_balance(int new_balance) {
        if (!is_frozen && new_balance >= min_balance && new_balance <= max_balance) {
            cur_balance = new_balance;
            return true;
        }
        return false;
    }

    int get_balance() const { return cur_balance; }
    int get_min_balance() const { return min_balance; }
    int get_max_balance() const { return max_balance; }

    void freeze(bool status) { is_frozen = status; }
    bool frozen() const { return is_frozen; }

    void set_min(int value) { min_balance = value; }
    void set_max(int value) { max_balance = value; }
};

// Represents the shared memory bank
class Bank {
private:
    size_t size;
    pthread_mutex_t lock;

public:
    Cell cells[];  // Flexible array member

    Bank(size_t n) : size(n) {
        // Initialize process-shared mutex
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&lock, &attr);
        pthread_mutexattr_destroy(&attr);

        // Placement new each Cell
        for (size_t i = 0; i < size; ++i) {
            new (&cells[i]) Cell();
        }
    }

    ~Bank() {
        // Manually destroy each Cell
        for (size_t i = 0; i < size; ++i) {
            cells[i].~Cell();
        }
        pthread_mutex_destroy(&lock);
    }

    void lock_bank() { pthread_mutex_lock(&lock); }
    void unlock_bank() { pthread_mutex_unlock(&lock); }

    Cell* get_cell(size_t index) {
        if (index >= size) return nullptr;
        return &cells[index];
    }

    void get_balance(int cell_id, const std::string& type) {
        lock_bank();

        if (cell_id < 0 || (size_t)cell_id >= size) {
            std::cerr << "Invalid cell index\n";
        } else if (type == "current") {
            std::cout << "Balance: " << cells[cell_id].get_balance() << std::endl;
        } else if (type == "min") {
            std::cout << "Min balance: " << cells[cell_id].get_min_balance() << std::endl;
        } else if (type == "max") {
            std::cout << "Max balance: " << cells[cell_id].get_max_balance() << std::endl;
        } else {
            std::cerr << "Invalid balance type" << std::endl;
        }

        unlock_bank();
    }

    bool transfer(size_t from_id, size_t to_id, int amount) {
	    if (from_id >= size || to_id >= size || from_id == to_id || amount < 0)
        	return false;

	    lock_bank();
	
	    Cell& from = cells[from_id];
	    Cell& to = cells[to_id];
	
	    int from_balance = from.get_balance();
	    int to_balance = to.get_balance();
	
	    
	    if (from.frozen() || to.frozen() ||
	        from_balance < amount ||
	        to_balance + amount > to.get_max_balance()) {
	        unlock_bank();
	        return false;
	    }
	
	    from.set_balance(from_balance - amount);
	    to.set_balance(to_balance + amount);

	    unlock_bank();
    	    return true;
    }
    void freeze_account(size_t id, bool freeze) {
    	if (id >= size) {
        	std::cerr << "Invalid account ID\n";
	        return;
	    }

	    lock_bank();
	    cells[id].freeze(freeze);
            std::cout << "Account " << id << (freeze ? " frozen" : " unfrozen") << ".\n";
	    unlock_bank();
    }


    void credit_all(int amount) {
        lock_bank();
        for (size_t i = 0; i < size; ++i) {
	    if (cells[i].frozen()) {
                std::cout << "Account " << i << " is frozen, skipping.\n";
                continue;
            }

            int current = cells[i].get_balance();
            int max_limit = cells[i].get_max_balance();
            int new_balance = current + amount;

            if (new_balance > max_limit) {
                std::cout << "Account " << i << " exceeds max balance, skipping.\n";
                continue;
            }

            cells[i].set_balance(new_balance);
            std::cout << "Credited " << amount << " to account " << i << ".\n";
        }
        unlock_bank();
    }

    void debit_all(int amount) {
        lock_bank();
        for (size_t i = 0; i < size; ++i) {
            if (cells[i].frozen()) {
                std::cout << "Account " << i << " is frozen, skipping.\n";
                continue;
            }

            int current = cells[i].get_balance();
            int min_limit = cells[i].get_min_balance();
            int new_balance = current - amount;

            if (new_balance < min_limit) {
                std::cout << "Account " << i << " falls below min balance, skipping.\n";
                continue;
            }

            cells[i].set_balance(new_balance);
            std::cout << "Debited " << amount << " from account " << i << ".\n";
        }
        unlock_bank();
    }
    void set_min(size_t id, int value) {
        if (id >= size) {
            std::cerr << "Invalid account ID\n";
            return;
        }

        lock_bank();
        cells[id].set_min(value);
        std::cout << "Min balance set to " << value << " for account " << id << ".\n";
        unlock_bank();
    }

    void set_max(size_t id, int value) {
        if (id >= size) {
            std::cerr << "Invalid account ID\n";
            return;
        }

        lock_bank();
        cells[id].set_max(value);
        std::cout << "Max balance set to " << value << " for account " << id << ".\n";
        unlock_bank();
    }
    size_t get_size() const { return size; }
};


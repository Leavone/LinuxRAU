#pragma once
#include <pthread.h>
#include <iostream>
#include <string>
#include <sstream>

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
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&lock, &attr);
        pthread_mutexattr_destroy(&attr);

        for (size_t i = 0; i < size; ++i) {
            new (&cells[i]) Cell();
        }
    }

    ~Bank() {
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

    void get_balance(int cell_id, const std::string& type, std::string& response) {
        lock_bank();

        std::ostringstream oss;
        if (cell_id < 0 || (size_t)cell_id >= size) {
            oss << "Invalid cell index\n";
        } else if (type == "current") {
            oss << "Balance: " << cells[cell_id].get_balance() << "\n";
        } else if (type == "min") {
            oss << "Min balance: " << cells[cell_id].get_min_balance() << "\n";
        } else if (type == "max") {
            oss << "Max balance: " << cells[cell_id].get_max_balance() << "\n";
        } else {
            oss << "Invalid balance type\n";
        }

        unlock_bank();
        response = oss.str();
    }

    bool transfer(size_t from_id, size_t to_id, int amount, std::string& response) {
        if (from_id >= size || to_id >= size || from_id == to_id || amount < 0) {
            response = "Invalid transfer parameters.\n";
            return false;
        }

        lock_bank();

        Cell& from = cells[from_id];
        Cell& to = cells[to_id];

        int from_balance = from.get_balance();
        int to_balance = to.get_balance();

        if (from.frozen() || to.frozen() ||
            from_balance < amount ||
            to_balance + amount > to.get_max_balance()) {
            unlock_bank();
            response = "Transfer failed due to constraints.\n";
            return false;
        }

        from.set_balance(from_balance - amount);
        to.set_balance(to_balance + amount);

        unlock_bank();
        response = "Transfer successful.\n";
        return true;
    }

    void freeze_account(size_t id, bool freeze, std::string& response) {
        std::ostringstream oss;
        if (id >= size) {
            oss << "Invalid account ID\n";
            response = oss.str();
            return;
        }

        lock_bank();
        cells[id].freeze(freeze);
        oss << "Account " << id << (freeze ? " frozen.\n" : " unfrozen.\n");
        unlock_bank();

        response = oss.str();
    }

    void credit_all(int amount, std::string& response) {
        lock_bank();
        std::ostringstream oss;

        for (size_t i = 0; i < size; ++i) {
            if (cells[i].frozen()) {
                oss << "Account " << i << " is frozen, skipping.\n";
                continue;
            }

            int current = cells[i].get_balance();
            int max_limit = cells[i].get_max_balance();
            int new_balance = current + amount;

            if (new_balance > max_limit) {
                oss << "Account " << i << " exceeds max balance, skipping.\n";
                continue;
            }

            cells[i].set_balance(new_balance);
            oss << "Credited " << amount << " to account " << i << ".\n";
        }

        unlock_bank();
        response = oss.str();
    }

    void debit_all(int amount, std::string& response) {
        lock_bank();
        std::ostringstream oss;

        for (size_t i = 0; i < size; ++i) {
            if (cells[i].frozen()) {
                oss << "Account " << i << " is frozen, skipping.\n";
                continue;
            }

            int current = cells[i].get_balance();
            int min_limit = cells[i].get_min_balance();
            int new_balance = current - amount;

            if (new_balance < min_limit) {
                oss << "Account " << i << " falls below min balance, skipping.\n";
                continue;
            }

            cells[i].set_balance(new_balance);
            oss << "Debited " << amount << " from account " << i << ".\n";
        }

        unlock_bank();
        response = oss.str();
    }

    void set_min(size_t id, int value, std::string& response) {
        std::ostringstream oss;
        if (id >= size) {
            oss << "Invalid account ID\n";
            response = oss.str();
            return;
        }

        lock_bank();
        cells[id].set_min(value);
        oss << "Min balance set to " << value << " for account " << id << ".\n";
        unlock_bank();

        response = oss.str();
    }

    void set_max(size_t id, int value, std::string& response) {
        std::ostringstream oss;
        if (id >= size) {
            oss << "Invalid account ID\n";
            response = oss.str();
            return;
        }

        lock_bank();
        cells[id].set_max(value);
        oss << "Max balance set to " << value << " for account " << id << ".\n";
        unlock_bank();

        response = oss.str();
    }

    size_t get_size() const { return size; }
};


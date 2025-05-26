#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/mman.h>
#include <fcntl.h>
#include "bank_for_remote.h"

#define SHM_NAME "/bank_shm"

Bank* attach_bank() {
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if(fd == -1) {
        perror("shm_open");
        return nullptr;
    }

    // Get bank size
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

void print_help(std::string& response) {
    response = "Available commands:\n"
               "  balance <account> [current|min|max]\n"
               "  freeze <account> [on|off]\n"
               "  transfer <from> <to> <amount>\n"
               "  credit_all <amount>\n"
               "  debit_all <amount>\n"
               "  set_min <account> <amount>\n"
               "  set_max <account> <amount>\n"
               "  help\n"
               "  exit\n";
}

bool process_command(Bank* bank, const std::string& cmd, std::string& response) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;

    if(action == "balance") {
        int account;
        std::string type;
        if(!(iss >> account >> type)) {
            response = "Invalid arguments\n";
            return false;
        }
        bank->get_balance(account, type, response);
	return true;
    }
    else if(action == "transfer") {
        int from, to, amount;
        if(!(iss >> from >> to >> amount) || amount <= 0) {
            response = "Invalid arguments\n";
            return false;
        }

        bank->transfer(from, to, amount, response); 
        return false;
    }
    else if(action == "freeze") {
        int account;
        std::string on_off;
        if(!(iss >> account >> on_off)) {
            response = "Invalid arguments\n";
            return false;
        }
        bank->freeze_account(account, on_off=="on", response);
        return true;
    }
    else if(action == "credit_all") {
        int amount;
        if(!(iss >> amount)) {
	    response =  "Invalid arguments\n";
            return false;
        }
        bank->credit_all(amount, response);
        return true;
    }
    else if(action == "debit_all") {
        int amount;
        if(!(iss >> amount)) {
            std::cerr << "Invalid arguments" << std::endl;
            return false;
        }
        bank->debit_all(amount, response);
        return true;
    }
    else if(action == "set_min") {
        int account, amount;
        if(!(iss >> account >> amount)) {
            std::cerr << "Invalid arguments" << std::endl;
            return false;
        }
        bank->set_min(account, amount, response);
        return true;
    }
    else if(action == "set_max") {
        int account, amount;
        if(!(iss >> account >> amount)) {
            std::cerr << "Invalid arguments" << std::endl;
            return false;
        }
        bank->set_max(account, amount, response);
        return true;
    }
    else if(action == "help") {
        print_help(response);
        return true;
    }
    else if(action == "exit") {
        return false;
    }
    else {
        response = "Unknown command\n";
        return false;
    }
}

constexpr int SERVER_PORT = 8888;
constexpr int MAX_CONNECTIONS = 10;

pthread_mutex_t request_counter_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t request_threshold_alert = PTHREAD_COND_INITIALIZER;
pthread_mutex_t output_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t client_connections_lock = PTHREAD_MUTEX_INITIALIZER;

int total_requests_processed = 0;
bool should_continue_running = true;

struct ClientConnectionData {
    int connection_fd;
    int server_fd;
    Bank* bank_ptr;
};



void* handle_client_connection(void* connection_data) {
    auto* data = static_cast<ClientConnectionData*>(connection_data);
    int client_fd = data->connection_fd;
    int main_server_fd = data->server_fd;
    Bank* bank_ptr = data->bank_ptr;
    delete data;
    
    char input_buffer[1024];

    while (true) {
        ssize_t received_bytes = recv(client_fd, input_buffer, sizeof(input_buffer) - 1, 0);
        if (received_bytes <= 0) {
            pthread_mutex_lock(&output_lock);
            std::cout << "Connection terminated by client\n";
            pthread_mutex_unlock(&output_lock);
            break;
        }

        input_buffer[received_bytes] = '\0';
        std::string client_command(input_buffer);

        if (client_command == "shutdown") {
            should_continue_running = false;
            close(client_fd);
            close(main_server_fd);
            exit(0);
        }

        std::string response;
        process_command(bank_ptr, client_command, response);

        pthread_mutex_lock(&client_connections_lock);
        ssize_t sent_bytes = send(client_fd, response.c_str(), response.size(), 0);
        pthread_mutex_unlock(&client_connections_lock);

        if (sent_bytes < 0) {
            perror("Failed to send response");
            break;
        }

        pthread_mutex_lock(&request_counter_lock);
        total_requests_processed++;
        if (total_requests_processed % 5 == 0) {
            pthread_cond_signal(&request_threshold_alert);
        }
        pthread_mutex_unlock(&request_counter_lock);
    }

    close(client_fd);
    return nullptr;
}

void* display_server_stats(void* arg) {
    while (should_continue_running) {
        pthread_mutex_lock(&request_counter_lock);
        pthread_cond_wait(&request_threshold_alert, &request_counter_lock);
        std::cout << "Total requests handled: " << total_requests_processed << "\n";
        pthread_mutex_unlock(&request_counter_lock);
    }
    return nullptr;
}

int initialize_server_connection() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    int option = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option))) {
        perror("Socket option setting failed");
        close(server_fd);
        return -1;
    }

    sockaddr_in server_config{};
    server_config.sin_family = AF_INET;
    server_config.sin_addr.s_addr = INADDR_ANY;
    server_config.sin_port = htons(SERVER_PORT);

    if (bind(server_fd, (sockaddr*)&server_config, sizeof(server_config)) < 0) {
        perror("Binding failed");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, MAX_CONNECTIONS) < 0) {
        perror("Listen failed");
        close(server_fd);
        return -1;
    }

    return server_fd;
}

int main() {
    Bank* bank = attach_bank();
    if(!bank) return EXIT_FAILURE;
    
    int main_server_fd = initialize_server_connection();
    if (main_server_fd < 0) {
        return EXIT_FAILURE;
    }

    std::cout << "Bank server operational on port " << SERVER_PORT << "\n";

    pthread_t stats_thread;
    pthread_create(&stats_thread, nullptr, display_server_stats, nullptr);

    while (should_continue_running) {
        sockaddr_in client_info{};
        socklen_t client_info_size = sizeof(client_info);

        int client_fd = accept(main_server_fd, (sockaddr*)&client_info, &client_info_size);
        if (client_fd < 0) {
            perror("Client connection acceptance failed");
            continue;
        }

        ClientConnectionData* connection_data = new ClientConnectionData{client_fd, main_server_fd, bank};
        pthread_t thread_id;
        if (pthread_create(&thread_id, nullptr, handle_client_connection, connection_data)) {
            perror("Thread creation failed");
            close(client_fd);
        }
        pthread_detach(thread_id);
    }

    close(main_server_fd);
    size_t size = sizeof(Bank) + bank->get_size() * sizeof(Cell);
    munmap(bank, size);
    return EXIT_SUCCESS;
}

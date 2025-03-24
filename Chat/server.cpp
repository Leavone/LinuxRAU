#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <vector>

struct Client {
	std::string username;
	int socket;
};

std::vector<Client> clients;

void send_messages(const char* message, int sender_socket) {
	for (auto item: clients) {
		if (item.socket != sender_socket) {
			send(item.socket, message, strlen(message), 0);
		}
	}
}

void* handle_client(void* arg) {
    int client_socket = (int)(intptr_t)arg;
    char buffer[1024];
    std::string username;

    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        close(client_socket);
        return nullptr;
    }
    buffer[bytes_received] = '\0';
    username = buffer;
    clients.push_back(Client{username, client_socket});

    std::string welcome_message = username + " has joined the chat!";
    send_messages(welcome_message.c_str(), client_socket);

    while (true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            break;
        }

        buffer[bytes_received] = '\0';

        std::cout << username << ": " << buffer << "\n";

	std::string message(buffer);

	if (message == "/exit") {
            break;
        }

	else if (message == "/list") {
            std::string user_list = "Connected users: ";
            for (const auto& client : clients) {
                user_list += "\n" + client.username;
            }
            send(client_socket, user_list.c_str(), user_list.size(), 0);
            continue;
        }
	else {
	   send_messages((username + ": " + message).c_str(), client_socket);
	}
    }
    
       for (auto it = clients.begin(); it != clients.end(); ) {
    	if (it->socket == client_socket) {
        	it = clients.erase(it);
    	} 
	else {
        	++it;
    	}
   
    }
   
    close(client_socket);
    send_messages((username + " left the chat.").c_str(), client_socket);
    return nullptr;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        return 1;
    }
    struct sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8888);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        close(server_socket);
        return 1;
    }


    while (true) {
        struct sockaddr_in client_address{};
        socklen_t client_addr_len = sizeof(client_address);

        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        pthread_t thread;
        if (pthread_create(&thread, nullptr, handle_client, (void*)(intptr_t)client_socket) != 0) {
            perror("Thread creation failed");
            close(client_socket);
        }
	pthread_detach(thread);
    }

    close(server_socket);
    return 0;
}

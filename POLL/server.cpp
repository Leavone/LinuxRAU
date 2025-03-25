#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <queue>
#include <poll.h>


pthread_mutex_t cout_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

std::queue<int> client_queue;

void* worker_thread(void*) {
    while (true) {
        pthread_mutex_lock(&queue_mutex);
        while (client_queue.empty()) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }
        
        int client_socket = client_queue.front();
        client_queue.pop();
        pthread_mutex_unlock(&queue_mutex);

        char buffer[1024];
        while (true) {
            int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received <= 0) {
                pthread_mutex_lock(&cout_mutex);
                std::cout << "Client disconnected\n";
                pthread_mutex_unlock(&cout_mutex);
                break;
            }
            
            buffer[bytes_received] = '\0';          
            
            pthread_mutex_lock(&cout_mutex);
            std::cout << "Hello, I am a server\n";
            pthread_mutex_unlock(&cout_mutex);
            
            std::string response = "Hello";
            send(client_socket, response.c_str(), response.size(), 0);
        }

        close(client_socket);
    }
    return nullptr;
}

int main() {
  // create a socket  
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
      perror("socket creation error");
      exit(errno);
  }

  // create an endpoint
  
  // socket address
  struct sockaddr_in server_address;
  // internet protocol = AF_INET
  server_address.sin_family = AF_INET;
  // accept or any address (bind the socket to all available interfaces)
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  // port
  server_address.sin_port = htons(8888);

  // Bind server_socket to server_address
  if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
      perror("bind failed");
      exit(errno);
  }

  // Listen for incoming connections
  if (listen(server_socket, 10) < 0) {
    perror("listen failed");
    exit(errno);
  }
  std::cout << "Waiting for connection\n";

  pthread_t thread_pool[10];
  for(int i = 0; i < 10; i++){
  	pthread_create(&thread_pool[i], nullptr, worker_thread, nullptr);
  }

  struct pollfd fds[100];
  fds[0].fd = server_socket;
  fds[0].events = POLLIN;
  int client_count = 1;

  while(true) {
      int poll_count = poll(fds, client_count, -1);
      if(poll_count < 0){
      	perror("failed to poll");
	break;
      }

      if(fds[0].revents & POLLIN){
      	int client_socket;
        struct sockaddr_in client_address;
        unsigned int client_addr_len = sizeof(client_address);
	if ((client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_addr_len)) < 0) {
          perror("accept failed");
          exit(errno);
        }
	pthread_mutex_lock(&cout_mutex);
        std::cout << "New client connected: " << inet_ntoa(client_address.sin_addr) << "\n";
	pthread_mutex_unlock(&cout_mutex);

	pthread_mutex_lock(&queue_mutex);
        client_queue.push(client_socket);
        pthread_cond_signal(&queue_cond);
        pthread_mutex_unlock(&queue_mutex);


      }
 }
  
  // close
  close(server_socket);
  return 0;
}


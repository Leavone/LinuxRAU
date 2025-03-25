#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <fcntl.h>

int main(){
  const std::string hostname = "httpforever.com";
  const std::string request = "GET / HTTP/1.1\r\nHost: httpforever.com\r\nConnection: close\r\n\r\n";
  char buffer[100];
  // server address 
  struct sockaddr_in server_address;

  // create a socket
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
        perror("socket creation error");
        exit(errno);
  }

  // fill the server ip and port address 
  server_address.sin_addr.s_addr = inet_addr("146.190.62.39");
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(80);

  // connect
  int connected = connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address));

  if(connected == -1){
    perror("connection failed");
    exit(errno);
  }

  // send message to server
  int sent = send(client_socket, request.c_str(), request.size(), 0);
  if(sent == -1){
    exit(errno);
  }
  int fd = open("httpforever.html", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if(fd == -1){
  	perror("Failed to open a file");
	exit(errno);
  }
  ssize_t bytes_received;
  while((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0){
  	write(fd, buffer, bytes_received);
  }
  if(bytes_received == -1){
  	perror("failed to receive message");
	exit(errno);
  }
  close(fd);
  close(client_socket);
  return 0;
}


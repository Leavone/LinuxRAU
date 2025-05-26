#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>
#include "colorprint/colorprint.hpp"

constexpr int PORT = 8888;

int main() {
    struct sockaddr_in server_address{};

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket creation error");
        exit(errno);
    }

    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    int connected = connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    if(connected == -1){
        perror("connection failed");
        exit(errno);
    }

    std::cout << "Connected to bank server.\n";
    std::vector<std::string> success = {"Credited", "Debited", "Balance:", "balance:", "Transfer successful.", "unfrozen.", "Min balance set to", "Max balance set to"};
    std::vector<std::string> failure = {"Transfer failed due to constraints.", "Invalid transfer parameters.", "falls below min balance, skipping.", "exceeds max balance, skipping.", "is frozen, skipping." , "Invalid balance type", "Invalid cell index", "Invalid account ID", "frozen"};
    Painter painter(std::cout, success, failure);

    while (true) {
        std::cout << "> ";
        std::string command;
        std::getline(std::cin, command);

        if (command.empty()) continue;

        if (command == "exit") break;

        if (send(client_socket, command.c_str(), command.size(), 0) < 0) {
            perror("send");
            break;
        }

        char buffer[1024];
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            perror("recv");
            break;
        }

        buffer[bytes_received] = '\0';
        std::string response(buffer);
        painter.printColoredLine(response);

    }

    close(client_socket);
    std::cout << "Disconnected.\n";
    return 0;
}

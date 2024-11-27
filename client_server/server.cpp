#include "../protocol/Protocol.h"
#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set port reusable
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    // Accept a connection
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Connection established with client.\n";

    while (true) {
        std::vector<Byte> buffer(1024, 0);
        int valread = read(new_socket, buffer.data(), buffer.size());
        if (valread <= 0) {
            std::cerr << "Client disconnected or error occurred.\n";
            break;
        }

        try {
            // Decode incoming message
            auto [command, args] = Protocol::decodeMessage(buffer);
            std::cout << "Decoded message received:\n";
            std::cout << "Command: " << command << "\nArguments: ";
            for (const auto& arg : args) {
                std::cout << arg << " ";
            }
            std::cout << "\n";

            // Send encoded response
            std::vector<Byte> response = Protocol::encodeResponse("OK", "Command executed successfully");
            send(new_socket, response.data(), response.size(), 0);
            std::cout << "Encoded response sent.\n";

        } catch (const std::exception& e) {
            std::cerr << "Error decoding message: " << e.what() << "\n";
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}

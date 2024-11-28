#include "../protocol/Protocol.h"
#include "../ExtendibleHashing.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <chrono>
#include <iomanip>
#include <ctime>

#define PORT 8080

// Helper function to get the current timestamp
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::ostringstream timestamp;
    timestamp << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %H:%M:%S");
    return timestamp.str();
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create and initialize the hash table
    ExtendibleHashing hashTable(1);

    // Open the log file
    std::ofstream logFile("server.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file.\n";
        return -1;
    }

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
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
            std::string response;

            // Log the timestamp and received command
            logFile << "[" << getCurrentTimestamp() << "] Received Command: " << command << "\nArguments: ";
            for (const auto& arg : args) {
                logFile << arg << " ";
            }
            logFile << "\n";

            if (command == "SET") {
                if (args.size() < 2) {
                    response = "Error: SET requires a key and a value";
                } else {
                    hashTable.insert(args[0], args[1]);
                    response = "SET operation successful";

                    // Log hash table state after SET
                    logFile << "[" << getCurrentTimestamp() << "] Hash Table After SET:\n";

                    // Redirect std::cout to the log file temporarily
                    std::streambuf* originalCout = std::cout.rdbuf();
                    std::cout.rdbuf(logFile.rdbuf());

                    // Print the hash table to the log file
                    hashTable.print();

                    // Restore original std::cout
                    std::cout.rdbuf(originalCout);
                }
            } else if (command == "GET") {
                if (args.size() < 1) {
                    response = "Error: GET requires a key";
                } else {
                    std::string value;
                    if (hashTable.search(args[0], value)) {
                        response = "GET operation successful: " + value;
                    } else {
                        response = "Error: Key not found";
                    }
                }
            } else if (command == "DEL") {
                if (args.size() < 1) {
                    response = "Error: DEL requires a key";
                } else {
                    hashTable.remove(args[0]);
                    response = "DEL operation acknowledged";
                }
            } else {
                response = "Error: Unknown command";
            }

            logFile << "[" << getCurrentTimestamp() << "] Response: " << response << "\n\n";
            logFile.flush(); // Ensure logs are written immediately

            // Send encoded response
            std::vector<Byte> encodedResponse = Protocol::encodeResponse("OK", response);
            send(new_socket, encodedResponse.data(), encodedResponse.size(), 0);
        } catch (const std::exception& e) {
            std::cerr << "Error decoding message: " << e.what() << "\n";
            logFile << "[" << getCurrentTimestamp() << "] Error decoding message: " << e.what() << "\n";
        }
    }

    close(new_socket);
    close(server_fd);
    logFile.close();
    return 0;
}

#include "../protocol/Protocol.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

#define PORT 8080

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket creation error\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address or address not supported\n";
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection failed\n";
        return -1;
    }

    std::cout << "Connected to server.\n";

    // Open operations file
    std::ifstream file("operations.txt");
    if (!file.is_open())
    {
        std::cerr << "Failed to open operations file. Check the file path.\n";
        perror("File open error");
        return -1;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty()) {
            continue; // Skip empty lines
        }

        std::istringstream iss(line);
        std::string command;
        std::vector<std::string> args;

        // Parse command and arguments
        iss >> command;
        std::string arg;
        while (iss >> arg)
        {
            args.push_back(arg);
        }

        try
        {
            // Encode command
            std::vector<Byte> encodedCommand = Protocol::encodeCommand(command, args);
            send(sock, encodedCommand.data(), encodedCommand.size(), 0);
            std::cout << "Encoded message sent: " << line << "\n";

            // Receive response
            std::vector<Byte> buffer(1024, 0);
            int valread = read(sock, buffer.data(), buffer.size());
            if (valread > 0)
            {
                auto [status, data] = Protocol::decodeResponse(buffer);
                std::cout << "Decoded response received:\n";
                std::cout << "Status: " << status << "\n";
                std::cout << "Data: " << data << "\n";
            }
            else
            {
                std::cerr << "Failed to receive response from server.\n";
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    file.close();
    close(sock);
    return 0;
}

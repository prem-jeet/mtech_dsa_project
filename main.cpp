#include <iostream>
#include <vector>

// Include the protocol implementation
#include "Protocol.cpp"

int main() {
    try {
        // Encode a command
        std::string command = "SET";
        std::vector<std::string> arguments = {"key", "value"};

        std::vector<Byte> encodedMessage = encodeCommand(command, arguments);

        std::cout << "Encoded Message: ";
        for (Byte b : encodedMessage) {
            printf("%02X ", b);
        }
        std::cout << std::endl;

        // Decode the message
        std::cout << "\nDecoded Message:" << std::endl;
        decodeCommand(encodedMessage);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

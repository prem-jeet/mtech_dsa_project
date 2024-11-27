#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <cstring>

// Alias for Byte type
using Byte = uint8_t;

// Helper function to append data to a buffer
void appendToBuffer(std::vector<Byte>& buffer, const void* data, size_t size) {
    const Byte* byteData = static_cast<const Byte*>(data);
    buffer.insert(buffer.end(), byteData, byteData + size);
}

// Encode a command into the binary protocol
std::vector<Byte> encodeCommand(const std::string& commandType, const std::vector<std::string>& arguments) {
    std::vector<Byte> buffer;

    // Command mapping
    std::unordered_map<std::string, Byte> commandMap = {
        {"SET", 0x01},
        {"GET", 0x02},
        {"DEL", 0x03}
    };

    if (commandMap.find(commandType) == commandMap.end()) {
        throw std::invalid_argument("Unknown command type: " + commandType);
    }

    Byte commandByte = commandMap[commandType];
    Byte numArguments = arguments.size();

    // Reserve space for the length field
    size_t lengthOffset = buffer.size();
    buffer.resize(buffer.size() + 2); // Placeholder for 2-byte length

    // Append command type
    buffer.push_back(commandByte);

    // Append number of arguments
    buffer.push_back(numArguments);

    // Append arguments
    for (const std::string& arg : arguments) {
        uint16_t argLength = arg.size();

        // Convert length to big-endian
        Byte bigEndianArgLength[2];
        bigEndianArgLength[0] = (argLength >> 8) & 0xFF; // MSB
        bigEndianArgLength[1] = argLength & 0xFF;        // LSB

        appendToBuffer(buffer, bigEndianArgLength, sizeof(bigEndianArgLength)); // Append argument length
        appendToBuffer(buffer, arg.data(), arg.size());                         // Append argument data
    }

    // Calculate total length (excluding length field)
    uint16_t totalLength = buffer.size() - 2;

    // Convert totalLength to big-endian
    Byte bigEndianLength[2];
    bigEndianLength[0] = (totalLength >> 8) & 0xFF; // MSB
    bigEndianLength[1] = totalLength & 0xFF;        // LSB

    // Insert the big-endian length at the start
    std::memcpy(buffer.data() + lengthOffset, bigEndianLength, sizeof(bigEndianLength));

    return buffer;
}

// Decode a binary message into components
void decodeCommand(const std::vector<Byte>& buffer) {
    size_t offset = 0;

    // Extract length (big-endian)
    uint16_t totalLength = (buffer[offset] << 8) | buffer[offset + 1];
    offset += 2;

    // Extract command type
    Byte commandByte = buffer[offset++];
    std::unordered_map<Byte, std::string> commandMap = {
        {0x01, "SET"},
        {0x02, "GET"},
        {0x03, "DEL"}
    };

    std::string commandType = commandMap[commandByte];

    // Extract number of arguments
    Byte numArguments = buffer[offset++];

    // Extract arguments
    std::vector<std::string> arguments;
    for (int i = 0; i < numArguments; ++i) {
        // Argument length (big-endian)
        uint16_t argLength = (buffer[offset] << 8) | buffer[offset + 1];
        offset += 2;

        // Argument data
        std::string argument(reinterpret_cast<const char*>(buffer.data() + offset), argLength);
        offset += argLength;

        arguments.push_back(argument);
    }

    // Print the decoded message
    std::cout << "Command Type: " << commandType << std::endl;
    std::cout << "Number of Arguments: " << (int)numArguments << std::endl;
    for (size_t i = 0; i < arguments.size(); ++i) {
        std::cout << "Argument " << i + 1 << ": " << arguments[i] << std::endl;
    }
}

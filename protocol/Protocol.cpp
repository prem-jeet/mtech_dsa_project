#include "Protocol.h"

std::vector<Byte> Protocol::encodeCommand(const std::string& command, const std::vector<std::string>& args) {
    std::vector<Byte> buffer;

    // Command mapping
    if (command == "SET") {
        buffer.push_back(0x01);
    } else if (command == "GET") {
        buffer.push_back(0x02);
    } else if (command == "DEL") {
        buffer.push_back(0x03);
    } else {
        throw std::invalid_argument("Unknown command: " + command);
    }

    // Encode number of arguments
    buffer.push_back(static_cast<Byte>(args.size()));

    // Encode each argument
    for (const auto& arg : args) {
        unsigned short length = static_cast<unsigned short>(arg.size());
        buffer.push_back(static_cast<Byte>(length >> 8)); // High byte
        buffer.push_back(static_cast<Byte>(length & 0xFF)); // Low byte
        buffer.insert(buffer.end(), arg.begin(), arg.end());
    }

    return buffer;
}

std::pair<std::string, std::vector<std::string>> Protocol::decodeMessage(const std::vector<Byte>& message) {
    size_t offset = 0;

    // Decode command
    std::string command;
    Byte commandByte = message[offset++];
    if (commandByte == 0x01) {
        command = "SET";
    } else if (commandByte == 0x02) {
        command = "GET";
    } else if (commandByte == 0x03) {
        command = "DEL";
    } else {
        throw std::invalid_argument("Unknown command byte");
    }

    // Decode number of arguments
    Byte numArgs = message[offset++];

    // Decode arguments
    std::vector<std::string> args;
    for (Byte i = 0; i < numArgs; ++i) {
        if (offset + 2 > message.size()) {
            throw std::runtime_error("Malformed message: incomplete argument length");
        }

        unsigned short length = (message[offset] << 8) | message[offset + 1];
        offset += 2;

        if (offset + length > message.size()) {
            throw std::runtime_error("Malformed message: argument exceeds message size");
        }

        args.emplace_back(message.begin() + offset, message.begin() + offset + length);
        offset += length;
    }

    return {command, args};
}

std::vector<Byte> Protocol::encodeResponse(const std::string& status, const std::string& data) {
    std::vector<Byte> buffer;

    // Encode status
    unsigned short statusLength = static_cast<unsigned short>(status.size());
    buffer.push_back(static_cast<Byte>(statusLength >> 8)); // High byte
    buffer.push_back(static_cast<Byte>(statusLength & 0xFF)); // Low byte
    buffer.insert(buffer.end(), status.begin(), status.end());

    // Encode data
    unsigned short dataLength = static_cast<unsigned short>(data.size());
    buffer.push_back(static_cast<Byte>(dataLength >> 8)); // High byte
    buffer.push_back(static_cast<Byte>(dataLength & 0xFF)); // Low byte
    buffer.insert(buffer.end(), data.begin(), data.end());

    return buffer;
}

std::pair<std::string, std::string> Protocol::decodeResponse(const std::vector<Byte>& response) {
    size_t offset = 0;

    // Decode status
    if (offset + 2 > response.size()) {
        throw std::runtime_error("Malformed response: incomplete status length");
    }
    unsigned short statusLength = (response[offset] << 8) | response[offset + 1];
    offset += 2;

    if (offset + statusLength > response.size()) {
        throw std::runtime_error("Malformed response: status exceeds response size");
    }
    std::string status(response.begin() + offset, response.begin() + offset + statusLength);
    offset += statusLength;

    // Decode data
    if (offset + 2 > response.size()) {
        throw std::runtime_error("Malformed response: incomplete data length");
    }
    unsigned short dataLength = (response[offset] << 8) | response[offset + 1];
    offset += 2;

    if (offset + dataLength > response.size()) {
        throw std::runtime_error("Malformed response: data exceeds response size");
    }
    std::string data(response.begin() + offset, response.begin() + offset + dataLength);

    return {status, data};
}

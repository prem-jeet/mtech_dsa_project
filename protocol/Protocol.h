#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

using Byte = unsigned char;

class Protocol {
public:
    // Encode a command with arguments into a binary format
    static std::vector<Byte> encodeCommand(const std::string& command, const std::vector<std::string>& args);

    // Decode a binary message into a command and arguments
    static std::pair<std::string, std::vector<std::string>> decodeMessage(const std::vector<Byte>& message);

    // Encode a response into a binary format
    static std::vector<Byte> encodeResponse(const std::string& status, const std::string& data);

    // Decode a response into status and data
    static std::pair<std::string, std::string> decodeResponse(const std::vector<Byte>& response);
};

#endif

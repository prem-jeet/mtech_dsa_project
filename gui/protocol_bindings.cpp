#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../protocol/Protocol.h"

namespace py = pybind11;

PYBIND11_MODULE(protocol, m) {
    m.doc() = "Python bindings for Protocol.cpp";

    // Expose encodeCommand
    m.def("encode_command", [](const std::string &command, const std::vector<std::string> &args) {
        auto encoded = Protocol::encodeCommand(command, args);
        return py::bytes(reinterpret_cast<const char*>(encoded.data()), encoded.size());
    }, py::arg("command"), py::arg("args"),
       "Encode a command and arguments into a binary format.");

    // Expose decodeMessage
    m.def("decode_message", [](const std::string &message) {
        std::vector<Byte> binary(message.begin(), message.end());
        return Protocol::decodeMessage(binary);
    }, py::arg("message"),
       "Decode a binary message into a command and arguments.");

    // Expose encodeResponse
    m.def("encode_response", [](const std::string &status, const std::string &data) {
        auto encoded = Protocol::encodeResponse(status, data);
        return py::bytes(reinterpret_cast<const char*>(encoded.data()), encoded.size());
    }, py::arg("status"), py::arg("data"),
       "Encode a response into a binary format.");

    // Expose decodeResponse
    m.def("decode_response", [](const std::string &response) {
        std::vector<Byte> binary(response.begin(), response.end());
        return Protocol::decodeResponse(binary);
    }, py::arg("response"),
       "Decode a binary response into status and data.");
}

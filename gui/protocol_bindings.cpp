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
    m.def("decode_message", [](const py::bytes &message) {
        std::string str_message = message;
        std::vector<Byte> binary(str_message.begin(), str_message.end());
        auto [command, args] = Protocol::decodeMessage(binary);
        return py::make_tuple(command, args);
    }, py::arg("message"),
       "Decode a binary message into a command and arguments.");

    // Expose encodeResponse
    m.def("encode_response", [](const std::string &status, const std::string &data) {
        auto encoded = Protocol::encodeResponse(status, data);
        return py::bytes(reinterpret_cast<const char*>(encoded.data()), encoded.size());
    }, py::arg("status"), py::arg("data"),
       "Encode a response into a binary format.");

    // Expose decodeResponse
    m.def("decode_response", [](const py::bytes &response) {
        std::string str_response = response;
        std::vector<Byte> binary(str_response.begin(), str_response.end());
        auto [status, data] = Protocol::decodeResponse(binary);
        return py::make_tuple(status, data);
    }, py::arg("response"),
       "Decode a binary response into status and data.");
}

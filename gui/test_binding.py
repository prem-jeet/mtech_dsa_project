import protocol

# Test encodeCommand
command = "SET"
args = ["key1", "value1"]
encoded = protocol.encode_command(command, args)
print(f"Encoded command: {encoded}")

# Test decodeMessage
decoded = protocol.decode_message(encoded)
print(f"Decoded command: {decoded}")

# Test encodeResponse
status = "OK"
message = "Operation successful"
encoded_response = protocol.encode_response(status, message)
print(f"Encoded response: {encoded_response}")

# Test decodeResponse
decoded_response = protocol.decode_response(encoded_response)
print(f"Decoded response: {decoded_response}")

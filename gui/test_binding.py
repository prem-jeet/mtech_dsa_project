import protocol

# Test encodeCommand
print("Testing encode_command...")
command = "SET"
args = ["key1", "value1", "key2", "value2"]
encoded_command = protocol.encode_command(command, args)
print(f"Encoded command: {encoded_command}")

# Test decodeMessage
print("\nTesting decode_message...")
decoded_command, decoded_args = protocol.decode_message(encoded_command)
print(f"Decoded command: {decoded_command}")
print(f"Decoded arguments: {decoded_args}")

# Test encodeResponse
print("\nTesting encode_response...")
status = "OK"
message = "Operation successful"
encoded_response = protocol.encode_response(status, message)
print(f"Encoded response: {encoded_response}")

# Test decodeResponse
print("\nTesting decode_response...")
decoded_status, decoded_data = protocol.decode_response(encoded_response)
print(f"Decoded response status: {decoded_status}")
print(f"Decoded response data: {decoded_data}")

# Additional tests
print("\nAdditional tests with different commands...")

# Test GET command
command_get = "GET"
args_get = ["key1"]
encoded_get = protocol.encode_command(command_get, args_get)
print(f"Encoded GET command: {encoded_get}")
decoded_get_command, decoded_get_args = protocol.decode_message(encoded_get)
print(f"Decoded GET command: {decoded_get_command}")
print(f"Decoded GET arguments: {decoded_get_args}")

# Test DEL command
command_del = "DEL"
args_del = ["key1"]
encoded_del = protocol.encode_command(command_del, args_del)
print(f"Encoded DEL command: {encoded_del}")
decoded_del_command, decoded_del_args = protocol.decode_message(encoded_del)
print(f"Decoded DEL command: {decoded_del_command}")
print(f"Decoded DEL arguments: {decoded_del_args}")

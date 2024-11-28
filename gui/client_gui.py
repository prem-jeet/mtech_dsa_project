import protocol  # Import the C++ protocol bindings
import socket
from tkinter import Tk, Label, Entry, Button, scrolledtext, messagebox, StringVar, OptionMenu, Frame


class ClientGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Client GUI")
        self.root.geometry("600x600")

        # Server connection setup
        self.server_address = ("127.0.0.1", 8080)
        self.client_socket = None
        self.connect_to_server()

        # Operation Selection
        Label(root, text="Operation:").grid(row=0, column=0, padx=5, pady=5, sticky="w")
        self.operation = StringVar(value="SET")
        operations = ["SET", "GET", "DEL"]
        OptionMenu(root, self.operation, *operations, command=self.update_inputs).grid(row=0, column=1, padx=5, pady=5, sticky="w")

        # Dynamic Input Area
        self.input_frame = Frame(root)
        self.input_frame.grid(row=1, column=0, columnspan=3, padx=5, pady=5)
        self.key_value_pairs = []  # To track input fields for SET operation
        self.update_inputs("SET")

        # Buttons
        Button(root, text="Send", command=self.send_command).grid(row=2, column=0, padx=5, pady=5)
        Button(root, text="Clear", command=self.clear_inputs).grid(row=2, column=1, padx=5, pady=5)

        # Response Display
        Label(root, text="Response:").grid(row=3, column=0, padx=5, pady=5, sticky="nw")
        self.response_text = scrolledtext.ScrolledText(root, width=70, height=20)
        self.response_text.grid(row=4, column=0, columnspan=3, padx=5, pady=5)

    def connect_to_server(self):
        try:
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client_socket.connect(self.server_address)
            messagebox.showinfo("Connection", "Connected to the server successfully.")
        except Exception as e:
            messagebox.showerror("Connection Error", f"Failed to connect to the server: {e}")

    def update_inputs(self, operation):
        """Update input fields based on the selected operation."""
        for widget in self.input_frame.winfo_children():
            widget.destroy()

        if operation == "SET":
            self.add_key_value_input()  # Add the first key-value pair by default
            Button(self.input_frame, text="Add More", command=self.add_key_value_input).grid(row=100, column=0, padx=5, pady=5, sticky="w")
        elif operation == "GET":
            Label(self.input_frame, text="Key:").grid(row=0, column=0, padx=5, pady=5, sticky="w")
            self.key_input = Entry(self.input_frame, width=30)
            self.key_input.grid(row=0, column=1, padx=5, pady=5)
        elif operation == "DEL":
            Label(self.input_frame, text="Key:").grid(row=0, column=0, padx=5, pady=5, sticky="w")
            self.key_input = Entry(self.input_frame, width=30)
            self.key_input.grid(row=0, column=1, padx=5, pady=5)

    def add_key_value_input(self):
        """Add a key-value pair input field for the SET operation."""
        row = len(self.key_value_pairs)
        key_label = Label(self.input_frame, text=f"Key {row + 1}:")
        key_label.grid(row=row, column=0, padx=5, pady=5, sticky="w")
        key_entry = Entry(self.input_frame, width=20)
        key_entry.grid(row=row, column=1, padx=5, pady=5)

        value_label = Label(self.input_frame, text=f"Value {row + 1}:")
        value_label.grid(row=row, column=2, padx=5, pady=5, sticky="w")
        value_entry = Entry(self.input_frame, width=20)
        value_entry.grid(row=row, column=3, padx=5, pady=5)

        self.key_value_pairs.append((key_entry, value_entry))

    def send_command(self):
        if self.client_socket is None:
            messagebox.showerror("Error", "Not connected to the server.")
            return

        try:
            operation = self.operation.get()

            if operation == "SET":
                args = []
                for key_entry, value_entry in self.key_value_pairs:
                    key = key_entry.get().strip()
                    value = value_entry.get().strip()
                    if key and value:
                        args.append(key)
                        args.append(value)
                if not args:
                    messagebox.showwarning("Warning", "At least one key-value pair is required for SET.")
                    return

            elif operation in ["GET", "DEL"]:
                key = self.key_input.get().strip()
                if not key:
                    messagebox.showwarning("Warning", f"Key is required for {operation}.")
                    return
                args = [key]

            # Use protocol.encode_command to encode the command
            encoded_command = protocol.encode_command(operation, args)
            self.client_socket.sendall(encoded_command)

            # Receive and decode the response using protocol.decode_response
            response = self.client_socket.recv(1024)
            status, message = protocol.decode_response(response.decode("utf-8"))
            self.response_text.insert("end", f"Command: {operation} {args}\nStatus: {status}\nMessage: {message}\n\n")
            self.response_text.yview("end")  # Auto-scroll to the end
        except Exception as e:
            messagebox.showerror("Error", f"Failed to send command: {e}")

    def clear_inputs(self):
        """Clear all inputs."""
        operation = self.operation.get()
        self.update_inputs(operation)

    def close_connection(self):
        if self.client_socket:
            self.client_socket.close()
        self.root.destroy()


if __name__ == "__main__":
    root = Tk()
    gui = ClientGUI(root)
    root.protocol("WM_DELETE_WINDOW", gui.close_connection)
    root.mainloop()

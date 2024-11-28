import protocol  # Import the C++ protocol bindings
import socket
from tkinter import Tk, Label, Entry, Button, scrolledtext, messagebox


class ClientGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Client GUI")
        self.root.geometry("600x400")

        self.server_address = ("127.0.0.1", 8080)  # Server IP and port
        self.client_socket = None
        self.connect_to_server()

        # GUI Layout
        Label(root, text="Command:").grid(row=0, column=0, padx=5, pady=5, sticky="w")
        self.command_entry = Entry(root, width=50)
        self.command_entry.grid(row=0, column=1, padx=5, pady=5)

        Button(root, text="Send", command=self.send_command).grid(row=0, column=2, padx=5, pady=5)
        Button(root, text="Clear", command=self.clear_command).grid(row=1, column=2, padx=5, pady=5)

        Label(root, text="Response:").grid(row=2, column=0, padx=5, pady=5, sticky="nw")
        self.response_text = scrolledtext.ScrolledText(root, width=70, height=15)
        self.response_text.grid(row=2, column=1, columnspan=2, padx=5, pady=5)

    def connect_to_server(self):
        try:
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client_socket.connect(self.server_address)
            messagebox.showinfo("Connection", "Connected to the server successfully.")
        except Exception as e:
            messagebox.showerror("Connection Error", f"Failed to connect to the server: {e}")

    def send_command(self):
        if self.client_socket is None:
            messagebox.showerror("Error", "Not connected to the server.")
            return

        raw_command = self.command_entry.get().strip()
        if not raw_command:
            messagebox.showwarning("Warning", "Command cannot be empty.")
            return

        try:
            # Parse the raw command into command name and arguments
            parts = raw_command.split()
            command = parts[0].upper()
            args = parts[1:]

            # Use protocol.encode_command to encode the command
            encoded_command = protocol.encode_command(command, args)
            self.client_socket.sendall(encoded_command)

            # Receive and decode the response using protocol.decode_response
            response = self.client_socket.recv(1024)
            status, message = protocol.decode_response(response.decode("utf-8"))
            self.response_text.insert("end", f"Command: {raw_command}\nStatus: {status}\nMessage: {message}\n\n")
            self.response_text.yview("end")  # Auto-scroll to the end
        except Exception as e:
            messagebox.showerror("Error", f"Failed to send command: {e}")

    def clear_command(self):
        self.command_entry.delete(0, "end")

    def close_connection(self):
        if self.client_socket:
            self.client_socket.close()
        self.root.destroy()


if __name__ == "__main__":
    root = Tk()
    gui = ClientGUI(root)
    root.protocol("WM_DELETE_WINDOW", gui.close_connection)
    root.mainloop()

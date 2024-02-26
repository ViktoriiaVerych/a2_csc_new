# About
This assignment focuses on a crucial aspect of software development: handling multiple clients
concurrently through the introduction of programming threads.
The project outlines a client-server file management system using `Winsock` for network communication in a Windows environment. 
Communication between the client and server is text-based, with commands and data transmitted over TCP sockets to ensure reliable delivery.

## Server Side
### Server Initialization:
- *Winsock Initialization:* Calls WSAStartup to initiate the use of the Winsock DLL.
- *Socket Creation and Binding:* Creates a TCP socket using `socket(AF_INET, SOCK_STREAM, 0)` and binds it to a specified port with bind.
- *Listening for Connections:* Starts listening for incoming connections using listen.

### Multi-Client Handling
The server is designed to handle multiple client connections concurrently. 
This is achieved by creating a new thread for each client connection. 
- When a client connects to the server, the accept function returns a new socket specific to this client.
- The server then spawns a new thread passing this socket as an argument to the thread function (handleClient).
- This allows the server to handle multiple clients simultaneously, with each client being served in its own thread.

### Threading
- Utilizes thread-safe operations to prevent data races and ensure that shared resources, like console output or shared variables, are accessed in a safe manner.
-  This is typically achieved by using mutexes `(std::mutex)` to synchronize access to shared resources.

### Commands Handling
Commands are text-based and sent from the client to the server, where they are received and parsed. 
The server identifies the command type by inspecting the received buffer and invokes the corresponding handler function for the command. 
This design allows for a flexible and extendable command structure, where new commands can be added by defining new handler functions.

# Client Side


## Available Commands
### `LIST`
- **Client Request:** Sends the "LIST" command, which is 4 bytes long, to request a list of available files in the server's directory assigned to the client.
- **Server Response:** Compiles a list of filenames in the client's directory and sends this list back as a string. The response size depends on the number and length of the filenames but is capped at 1024 bytes per message, including newline characters for separation.
- **EXAMPLE:** `LIST`

### `PUT`
- **Client Request:** Begins with the command "PUT" followed by the filename. The client then sends the file size and the file data in chunks of up to 1024 bytes.
- **Server Response:** Upon successfully receiving and storing the file, the server sends a confirmation message indicating success or failure.
- - **EXAMPLE:** `PUT doc.docx`

### `GET`
- **Client Request:** The "GET" command followed by the filename instructs the server to send the requested file.
- **Server Response:** The server first sends the file size and then transmits the file content in chunks of up to 1024 bytes. The filename is sent separately.
- - **EXAMPLE:** `GET doc.docx`

### `DELETE`
- **Client Request:** The "DELETE" command followed by the filename requests the server to delete the specified file.
- **Server Response:** The server attempts to delete the file and returns a success or failure message.
- - **EXAMPLE:** `DELETE doc.docx`

### `EXIT`
- **Client Request:** The "EXIT" command is sent to gracefully terminate the connection.
- **Server Response:** The server acknowledges the request, leading to the termination of the connection loop on the client side.
- - **EXAMPLE:** `EXAMPLE`

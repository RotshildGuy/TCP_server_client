# TCP Chat Server and Client in C++

## Table of Contents

* [Overview](#overview)
* [Features](#features)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
* [Usage](#usage)
    * [Running the Server](#running-the-server)
    * [Running the Client(s)](#running-the-clients)
    * [Using the Chat](#using-the-chat)
    * [Shutting Down the Server](#shutting-down-the-server)
* [Code Explanation](#code-explanation)
    * [Server (`server.cpp`)](#server-servercpp)
    * [Client (`client.cpp`)](#client-clientcpp)
* [Potential Improvements](#potential-improvements)
* [Author](#author)
* [License](#license)

## Overview

This project is a simple implementation of a multi-client chat application using TCP sockets in C++. It provides a basic framework for understanding network programming concepts, including socket creation, binding, listening, accepting connections, and sending/receiving data. The application consists of two main components: a server and a client. The server manages client connections and facilitates communication between them, while the client provides a user interface for connecting to the server and sending/receiving messages.

## Features

* **Multiple Client Support:** The server can handle multiple concurrent client connections using threads, allowing for simultaneous communication between multiple users.
* **Name Registration:** Clients are required to choose a unique name upon connecting to the server. The server validates the uniqueness of the name and rejects connections with duplicate names.
* **Private Messaging:** Clients can send private messages to specific online users using the `send <recipient_name> <message>` command. This allows for direct communication between two users without broadcasting to the entire chat room.
* **List Online Users:** Clients can view a list of currently connected users using the `list` command. This provides users with an overview of who is currently online and available to chat.
* **Exit Command:** Clients can gracefully disconnect from the server using the `exit` command. This ensures a clean disconnection process, releasing server resources.
* **Server Shutdown:** The server can be shut down by typing `exit` in the server console. Upon shutdown, the server notifies all connected clients before closing the connections and terminating the application.

## Prerequisites

* **C++ Compiler:** A C++ compiler that supports C++11 is required (e.g., g++).
* **Operating System:** The code is designed to be compatible with operating systems that support standard socket programming (e.g., Linux, macOS).

## Installation

To compile and run the application, follow these steps:

1.  Open a terminal window.
2.  Navigate to the directory containing the `server.cpp` and `client.cpp` files.

### Server

Compile the server code using the following command:

```bash
g++ -std=c++11 server.cpp -o server -pthread
```

This command compiles the `server.cpp` file, creates an executable named `server`, and links the necessary thread library (`-pthread`).

### Client

Compile the client code using the following command:

```bash
g++ -std=c++11 client.cpp -o client -pthread
```

This command compiles the `client.cpp` file, creates an executable named `client`, and links the thread library.

## Usage

### Running the Server

1.  Open a terminal window.
2.  Navigate to the directory where you compiled the server executable.
3.  Run the server:

```bash
./server
```

The server will start listening for incoming client connections on IP address `127.0.0.1` and port `4444`. A message will be displayed in the terminal indicating that the server is listening.

### Running the Client(s)

1.  Open one or more new terminal windows for each client you want to run.
2.  Navigate to the directory where you compiled the client executable.
3.  Run the client, providing the server IP address and the desired username as command-line arguments:

```bash
./client <SERVER_IP> <USERNAME>
```

* `SERVER_IP`: The IP address of the machine where the server is running. If the server is running on the same machine, use `127.0.0.1`.
* `USERNAME`: The username you want to use for the client in the chat session. This name must be unique.

For example, to run two clients with usernames "Alice" and "Bob" on the same machine as the server, you would open two terminal windows and execute the following commands:

**Terminal 1 (Alice):**

```bash
./client 127.0.0.1 Alice
```

**Terminal 2 (Bob):**

```bash
./client 127.0.0.1 Bob
```

The client will attempt to connect to the server. If the username is unique, a "Connected to the server as: `<USERNAME>`" message will be displayed. If the username is already taken, the client will exit, and you will need to choose a different username.

### Using the Chat

Once connected to the server, clients can communicate with each other using the following commands:

* `send <recipient_name> <message>`: Sends a private message to the user with the specified `<recipient_name>`.
    * `<recipient_name>`: The username of the user to whom you want to send the message.
    * `<message>`: The message you want to send.

    Example:

```bash
Enter message: send Bob Hello Bob, how are you?
```

    This command will send the message "Hello Bob, how are you?" directly to the client with the username "Bob".

* `list`: Requests and displays a list of currently online users.

    Example:

```
Enter message: list
```

    The server will respond with a comma-separated list of usernames currently connected to the server.

* `exit`: Disconnects the client from the server and closes the client application.

    Example:

```
Enter message: exit
```

    This command will terminate the client's connection to the server and close the client program.

### Shutting Down the Server

1.  Open the terminal window where the server is running.
2.  Type `exit` and press Enter:

```bash
exit
```

The server will send a shutdown message to all connected clients, close all client connections, close the listening socket, and terminate the server application.

## Code Explanation

### Server (`server.cpp`)

The server code (`server.cpp`) performs the following key functions:

* **Socket Creation and Binding:**
    * Creates a TCP socket using the `socket()` function.
    * Binds the socket to a specific IP address and port using the `bind()` function. This associates the socket with a network address, allowing it to listen for incoming connections.
* **Listening for Connections:**
    * Puts the socket into listening mode using the `listen()` function. This allows the server to passively wait for clients to establish connections. The backlog parameter specifies the maximum number of pending connections the socket can handle.
* **Accepting Connections:**
    * When a client attempts to connect, the `accept()` function creates a new socket for communication with that specific client. This new socket is used for sending and receiving data with the client.
* **Client Handling Thread:**
    * Each connected client is handled by a separate thread, created using `std::thread`. The `handle_client()` function is executed in each thread, allowing the server to manage multiple clients concurrently. This is crucial for a chat server to handle multiple users simultaneously.
* **Name Registration:**
    * Upon establishing a connection, the client sends its desired username to the server.
    * The server receives this name and checks if it already exists in the list of connected usernames.
    * If the name is a duplicate, the server sends an error message to the client, closes the connection, and the client is not added to the chat.
    * If the name is unique, the server sends a success message to the client and adds the client's socket and name to the lists.
* **Message Handling:**
    * The `handle_client` function continuously listens for incoming messages from its connected client using the `recv()` function.
    * The `recv()` function blocks until data is available, or an error occurs.
* **Broadcasting (Private Messages):**
    * The `broadcast_message()` function sends a message to a specific client, given the client's socket.
    * The server parses incoming messages to determine the recipient. Messages starting with "send" are interpreted as private message requests.
    * The server extracts the recipient's name from the message, searches for the corresponding socket, and sends the message to that socket.
* **Listing Users:**
    * When a client sends the "list" command, the server retrieves the list of currently connected usernames.
    * The server formats this list (e.g., as a comma-separated string) and sends it back to the requesting client.
* **Server Chat Thread:**
    * The `server_chat` function runs in a separate thread.
    * It allows the server administrator to interact with the server from the console.
    * Currently, the only supported command is `exit`, which initiates the server shutdown process.
* **Client Disconnection:**
    * When a client sends the "exit" command or the connection is lost (e.g., due to a network error), the `handle_client` function detects this.
    * The server closes the client's socket using the `close()` function, removes the client's socket from the `clients` vector, and removes the client's name from the `names` vector. This ensures that the server maintains an accurate record of connected clients and cleans up resources.

### Client (`client.cpp`)

The client code (`client.cpp`) performs the following functions:

* **Socket Creation and Connection:**
    * Creates a TCP socket using the `socket()` function.
    * Connects to the server's IP address and port using the `connect()` function.
* **Sending Name:**
    * After successfully connecting to the server, the client sends its chosen username to the server using the `send()` function.
* **Receiving Messages Thread:**
    * A separate thread, created using `std::thread` and executing the `recieve_message()` function, is responsible for continuously listening for incoming messages from the server.
    * The `recv()` function is used to receive data from the server.
    * Received messages are displayed to the user on the console.
    * The thread also checks for a server shutdown message. If received, the client closes its socket and exits.
* **Sending Messages:**
    * The main thread of the client program reads user input from the console using `getline()`.
    * The entered messages are sent to the server using the `send()` function.
* **Handling Server Shutdown:**
    * The `recieve_message` thread checks for a specific shutdown message from the server.
    * If this message is received, the client closes its socket using `close()` and terminates gracefully.

## Potential Improvements

This chat application provides a basic foundation and can be extended with several features and improvements, including:

* **Error Handling:** Implement more robust error handling for network operations, such as handling connection errors, socket errors, and invalid data. This will make the application more resilient to unexpected situations.
* **Message Formatting:** Improve the formatting and presentation of messages displayed to clients. This could include adding timestamps, user colors, or other visual enhancements to make the chat more user-friendly.
* **Centralized Client Management:** Use a more robust and efficient data structure for managing client sockets and names. Consider using a `std::map` to associate names with sockets. Also, implement proper synchronization mechanisms, such as `std::mutex`, to protect shared data structures (e.g., the client list) from race conditions that can occur when multiple threads access the data concurrently.
* **Broadcast to All:** Implement a command (e.g., `broadcast <message>`) to send messages to all connected clients. This would allow for general chat room communication in addition to private messaging.
* **More Sophisticated Commands:** Add more commands to support additional functionalities, such as:
    * `/help`: Displays a list of available commands.
    * `/me <action>`: Displays an action performed by the user (e.g., "/me enters the room").
    * File transfer: Allow users to send files to each other.
* **Logging:** Implement logging of server activity and client interactions. This can be useful for debugging, monitoring server usage, and auditing chat conversations. Use a logging library like `spdlog` for efficient and flexible logging.
* **GUI:** Develop a graphical user interface (GUI) for

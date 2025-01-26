#include <iostream>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <thread>

using namespace std;

void recieve_message(int client_socket) {
    char buffer[1024];
    while(true){
        // Receive response from the server
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received > 0) {
            cout << "\r" << string(50,' ') <<  "\r" << buffer << endl;
            if(strcmp(buffer,"The server has shut down, good bye!") == 0){
                close(client_socket);
                _exit(1);
            }
            cout << "Enter message: " << flush;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <IP> <NAME>" << endl;
        return 1;
    }

    string name = argv[2];
    const char *ip = argv[1];
    int port = 4444;

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        cerr << "Failed to create socket." << endl;
        return 1;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    if (connect(client_socket, (sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        cerr << "Failed to connect to server." << endl;
        return 1;
    }

    // Send the client's name to the server
    send(client_socket, name.c_str(), name.size(), 0);

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);


    if (bytes_received > 0) {
        if (buffer[0] == '0') {
            cout << "Client name: " << name << " is already used." << endl;
            close(client_socket);
            return 0;
        } else {
            cout << "Connected to the server as: " << name << endl;
        }
    }

    thread receive_thread(recieve_message, client_socket);

    while (true) {
        string message;
        cout << "Enter message: ";
        getline(cin, message);

        // Send message to the server
        send(client_socket, message.c_str(), message.size(), 0);

        if (message == "exit") {
            break;
        }
    }

    close(client_socket);
    receive_thread.detach();
    return 0;
}
// g++ -std=c++11 client.cpp -o client

//./client <NAME> <IP> <PORT>

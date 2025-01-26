#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <algorithm>
#include <sstream>

using namespace std;

// Global list of client sockets (no protection for simplicity)
vector<int> clients;
vector<string> names;

void broadcast_message(const string &message, int reciver_socket) {
    send(reciver_socket, message.c_str(), message.size(), 0);
}

void handle_client(int client_socket) {
    char buffer[1024];
    string name;

    // Receive the client's name
    /*memset is a standard C function used to set a block of memory to a specific value. 
    It's commonly used to initialize or reset memory for arrays, structures, or buffers
     to ensure they start with a known state, such as all zeros.*/
    memset(buffer, 0, sizeof(buffer));
    if (recv(client_socket, buffer, sizeof(buffer), 0) > 0) {
        name = buffer;
    } else {
        close(client_socket);
        return;
    }
    if (count(names.begin(), names.end(), name) > 0) {
        string s = "0";
        send(client_socket, s.c_str(), s.size(), 0);
        close(client_socket);
        return;
    } else {
        string s = "1";
        send(client_socket, s.c_str(), s.size(), 0);
    }

    cout << "Client connected: " << name << endl;
    clients.push_back(client_socket); // Add client to the global list
    names.push_back(name);

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        // If the client disconnects or sends "exit"
        if (bytes_received <= 0 || string(buffer).find("exit") == 0) {
            cout << "Client disconnected: " << name << endl;
            close(client_socket);

            // Remove the client from the list
            clients.erase(remove(clients.begin(), clients.end(), client_socket), clients.end());
            names.erase(remove(names.begin(),names.end(),name), names.end());
            break;
        }

        // Create the message to broadcast

        string message = buffer;
        vector<string> words;
        istringstream iss(message);
        string word;
        while(iss >> word){
            words.push_back(word);
        }
        transform(words[0].begin(), words[0].end(), words[0].begin(), [] (unsigned char c) {return tolower(c);});
        if (words[0] == "send" && words.size() >= 2) {
            string rec_name = words[1];
            transform(rec_name.begin(), rec_name.end(), rec_name.begin(), [] (unsigned char c) {return tolower(c);});
        
            auto it = find(names.begin(), names.end(), words[1]);
            if (it != names.end()) {
                int index = distance(names.begin(), it);

                string msg = "Incoming message from " + name + ": ";
                for (int i = 2; i < words.size(); i++) {
                    msg += words[i] + " ";
                }
                // Send the message to the other clients
                broadcast_message(msg, clients[index]);
            } else {
                string s = "Client " + words[1] + " not found.";
                send(client_socket, s.c_str(), s.size(), 0);
            }
        } else if (words[0] == "list") {
            string s = "";
            for (string c : names) {
                s += c +", ";
            }
            broadcast_message(s, client_socket);
        } else {
            string s = "The legal commands are: send <name> <msg> , list, exit";
            broadcast_message(s, client_socket);
        } 
    }
}

void server_chat(int server_socket){
    string msg;
    getline(cin, msg);

    if (msg == "exit"){
        for (int client_socket : clients) {
            string s = "The server has shut down, good bye!";
            broadcast_message(s, client_socket);
            close(client_socket);
        }
        close(server_socket);
        _exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 1) {
        cerr << "Usage: " << argv[0] << endl;
        return 1;
    }

    const char *ip = "127.0.0.1";
    int port = 4444;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        cerr << "Failed to create socket." << endl;
        return 1;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        cerr << "Bind failed." << endl;
        return 1;
    }

    if (listen(server_socket, 10) == -1) {
        cerr << "Listen failed." << endl;
        return 1;
    }

    cout << "Server listening on " << ip << ":" << port << endl;

    thread server_thread(server_chat, server_socket);

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_size = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr *)&client_addr, &client_size);

        if (client_socket == -1) {
            cerr << "Failed to accept connection." << endl;
            continue;
        }

        // Handle the client in a new thread
        thread(handle_client, client_socket).detach();
    }

    close(server_socket);
    server_thread.detach();
    return 0;
}




//g++ -std=c++11 server.cpp -o server -pthread
//./server <IP> <PORT>

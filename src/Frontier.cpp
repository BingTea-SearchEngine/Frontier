#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <cstring>
#include <algorithm>

#include "PriorityQueue.hpp"

const std::string SOCKET_PATH = "/tmp/frontier";
const int MAX_CLIENTS = 10;

using std::cout, std::endl;

int main() {
    int serverSock, maxFd;
    struct sockaddr_un serverAddr;
    fd_set masterSet, readFds;
    std::vector<int> clientSockets;

    // Create the server socket
    serverSock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverSock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Remove file if it already exists
    unlink(SOCKET_PATH.c_str());

    // Bind with socket
    serverAddr.sun_family = AF_UNIX;
    strncpy(serverAddr.sun_path, SOCKET_PATH.c_str(), sizeof(serverAddr.sun_path) - 1);
    if (bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Listen from socket
    if (listen(serverSock, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        return 1;
    }

    FD_ZERO(&masterSet);
    FD_SET(serverSock, &masterSet);
    maxFd = serverSock;

    while (true) {
        readFds = masterSet;
        // Select blocks until new connection
        if (select(maxFd + 1, &readFds, nullptr, nullptr, nullptr) < 0) {
            perror("select() failed");
            break;
        }

        // Accept new connections
        if (FD_ISSET(serverSock, &readFds)) {
            struct sockaddr_un clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            int clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &clientLen);

            if (clientSock < 0) {
                perror("Accept failed");
            } else {
                cout << "New client connected! Socket FD: " << clientSock << endl;
                FD_SET(clientSock, &masterSet);
                clientSockets.push_back(clientSock);
                maxFd = std::max(maxFd, clientSock);
            }
        }

        // Check for messages from client connections
        for (auto it = clientSockets.begin(); it != clientSockets.end();) {
            int client_sock = *it;

            if (FD_ISSET(client_sock, &readFds)) {
                char buffer[256];
                int bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);

                if (bytes_received <= 0) {
                    // Client disconnected
                    cout << "Client disconnected: " << client_sock << endl;
                    close(client_sock);
                    FD_CLR(client_sock, &masterSet);
                    it = clientSockets.erase(it);
                } else {
                    buffer[bytes_received] = '\0';
                    cout << "Received from client " << client_sock << ": " << buffer << endl;

                    // Send response back
                    const char* response = "Hello from server";
                    send(client_sock, response, strlen(response), 0);
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }

    close(serverSock);
    unlink(SOCKET_PATH.c_str());
}

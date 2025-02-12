#include "Frontier.hpp"

Frontier::Frontier(std::string socketPath, int maxClients)
    : _socketPath(socketPath), MAX_CLIENTS(maxClients) {
    cout << socketPath << " " << maxClients << endl;
    // Create the server socket
    _serverSock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (_serverSock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Remove file if it already exists
    unlink(_socketPath.c_str());

    // Bind with socket
    _serverAddr.sun_family = AF_UNIX;
    strncpy(_serverAddr.sun_path, _socketPath.c_str(),
            sizeof(_serverAddr.sun_path) - 1);
    if (bind(_serverSock, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr)) <
        0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen from socket
    if (listen(_serverSock, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
}

Frontier::~Frontier() {
    close(_serverSock);
    unlink(_socketPath.c_str());
}

void Frontier::start() {
    FD_ZERO(&_masterSet);
    FD_SET(_serverSock, &_masterSet);
    _maxFd = _serverSock;

    while (true) {
        _readFds = _masterSet;
        // Select blocks until new connection
        if (select(_maxFd + 1, &_readFds, nullptr, nullptr, nullptr) < 0) {
            perror("select() failed");
            break;
        }

        // Accept new connections
        if (FD_ISSET(_serverSock, &_readFds)) {
            struct sockaddr_un clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            int clientSock =
                accept(_serverSock, (struct sockaddr*)&clientAddr, &clientLen);

            if (clientSock < 0) {
                perror("Accept failed");
            } else {
                cout << "New client connected! Socket FD: " << clientSock
                     << endl;
                FD_SET(clientSock, &_masterSet);
                _clientSockets.push_back(clientSock);
                _maxFd = std::max(_maxFd, clientSock);
            }
        }

        // Check for messages from client connections
        for (auto it = _clientSockets.begin(); it != _clientSockets.end();) {
            int clientSock = *it;

            if (FD_ISSET(clientSock, &_readFds)) {
                if (_handleClient(clientSock) <= 0) {
                    it = _clientSockets.erase(it);
                } else {
                    it++;
                }
            } else {
                ++it;
            }
        }
    }
}

int Frontier::_handleClient(int clientSock) {
    char buffer[256];
    int bytesReceived = recv(clientSock, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived <= 0) {
        // Client disconnected
        cout << "Client disconnected: " << clientSock << endl;
        close(clientSock);
        FD_CLR(clientSock, &_masterSet);
    } else {
        std::vector<std::string> test = {"google.com", "wikipedia.com",
                                         "umich.edu", "eecs.umich.edu"};
        buffer[bytesReceived] = '\0';
        cout << "Received from client " << clientSock << ": " << buffer << endl;

        // Send response back
        const char* response = "Hello from server";
        send(clientSock, response, strlen(response), 0);
    }
    return bytesReceived;
}

int main() {
    Frontier frontier;

    frontier.start();
}

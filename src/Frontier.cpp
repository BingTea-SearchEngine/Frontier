#include "Frontier.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/fmt/bundled/ranges.h>

Frontier::Frontier(std::string socketPath, int maxClients)
    : _socketPath(socketPath), MAX_CLIENTS(maxClients) {
    // Create the server socket
    _serverSock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (_serverSock < 0) {
        spdlog::error("Socket creation failed");
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
        spdlog::error("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen from socket
    if (listen(_serverSock, MAX_CLIENTS) < 0) {
        spdlog::error("Listen failed");
        exit(EXIT_FAILURE);
    }
}

Frontier::~Frontier() {
    close(_serverSock);
    unlink(_socketPath.c_str());
}

std::string Frontier::getInfo() {
    return "Listening on " + _socketPath + " with max client " + std::to_string(MAX_CLIENTS);
}

void Frontier::start() {
    FD_ZERO(&_masterSet);
    FD_SET(_serverSock, &_masterSet);
    _maxFd = _serverSock;

    while (true) {
        _readFds = _masterSet;
        // Select blocks until new connection
        if (select(_maxFd + 1, &_readFds, nullptr, nullptr, nullptr) < 0) {
            spdlog::error("select() failed");
        }

        // Accept new connections
        if (FD_ISSET(_serverSock, &_readFds)) {
            struct sockaddr_un clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            int clientSock =
                accept(_serverSock, (struct sockaddr*)&clientAddr, &clientLen);

            if (clientSock < 0) {
                spdlog::error("Accept failed");
            } else {
                spdlog::info("==== New client connected: " + std::to_string(clientSock) + " ====");
                FD_SET(clientSock, &_masterSet);
                _clientSockets.push_back(clientSock);
                _maxFd = std::max(_maxFd, clientSock);
            }
        }

        // Check for messages from client connections
        for (auto it = _clientSockets.begin(); it != _clientSockets.end();) {
            int clientSock = *it;

            struct timeval timeout;
            timeout.tv_sec = 5;  // 5 seconds timeout
            timeout.tv_usec = 0;
            setsockopt(clientSock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

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
    spdlog::info(">>> Request Start (Client: {})", clientSock);

    uint32_t messageLength = 0;
    int bytesReceived = 0;
    // Get message size
    bytesReceived += recv(clientSock, &messageLength, sizeof(messageLength), 0);
    if (bytesReceived <= 0 ){
        spdlog::info("<<< Request End (Client: {})", clientSock);
        spdlog::info("==== Client disconnected: " + std::to_string(clientSock) + " ====");
        close(clientSock);
        FD_CLR(clientSock, &_masterSet);
        return 0;
    }

    messageLength = ntohl(messageLength);
    if (messageLength > 0) {
        spdlog::info("Client {} with message length {}", clientSock, messageLength);
        std::string message(messageLength, '\0');
        // Get message
        if (recv(clientSock, message.data(), messageLength, MSG_WAITALL) <= 0) {
            spdlog::info("Error getting client message: " + std::to_string(clientSock));
            close(clientSock);
            FD_CLR(clientSock, &_masterSet);
            return 0;
        }
        std::vector<std::string> received = FrontierInterface::Decode(message);
        spdlog::info("Received {}", received);
    }

    std::vector<std::string> urls = {"google.com", "wikipedia.com", "https://github.com/wbjin"};

    // Send response back
    std::string response = FrontierInterface::Encode(urls);
    spdlog::info("Sending {} {}", response.size(), urls);
    uint32_t responseSize = htonl(response.size());
    send(clientSock, &responseSize, sizeof(responseSize), 0);
    send(clientSock, response.data(), response.size(), 0);
    spdlog::info("<<< Request End (Client: {})", clientSock);
    return bytesReceived;
}

int main() {
    spdlog::info("======= Frontier Started =======");
    Frontier frontier;
    spdlog::info(frontier.getInfo());

    frontier.start();
}

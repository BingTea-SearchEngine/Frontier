#pragma once

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>

#include "FrontierInterface.hpp"
#include "PriorityQueue.hpp"

using std::cout, std::endl;

class Frontier {
public:
    Frontier(std::string socketPath = "/tmp/frontier", int MAX_CLIENTS = 10);

    void start();

    ~Frontier();

private:
    PriorityQueue<std::string> _pq;

    std::vector<int> _clientSockets;
    struct sockaddr_un _serverAddr;
    fd_set _masterSet;
    fd_set _readFds;
    int _serverSock;
    int _maxFd;

    const std::string _socketPath;
    const int MAX_CLIENTS = 10;

    int _handleClient(int clientSock);
};

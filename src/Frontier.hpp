#pragma once

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

#include "BloomFilter.hpp"
#include "FrontierInterface.hpp"
#include "PriorityQueue.hpp"

using std::cout, std::endl;

class Frontier {
   public:
    Frontier(std::string socketPath, int MAX_CLIENTS, uint32_t maxUrls,
             std::string saveFile);

    std::string getInfo();

    void recoverFilter(const char* filePath);

    void start();

    ~Frontier();

   private:
    PriorityQueue _pq;
    BloomFilter _filter;

    std::vector<int> _clientSockets;
    struct sockaddr_un _serverAddr;
    fd_set _masterSet;
    fd_set _readFds;
    int _serverSock;
    int _maxFd;

    const std::string _socketPath;
    const int MAX_CLIENTS = 10;

    uint32_t numUrls = 0;

    int _handleClient(int clientSock);
};

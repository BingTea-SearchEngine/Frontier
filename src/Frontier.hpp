#pragma once

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>

#include "GatewayServer.hpp"
#include "BloomFilter.hpp"
#include "FrontierInterface.hpp"
#include "PriorityQueue.hpp"

using std::cout, std::endl;

class Frontier {
   public:
    Frontier(int port, int MAX_CLIENTS, uint32_t maxUrls, int batchSize, std::string seedList, 
             std::string saveFile);

    void recoverFilter(const char* filePath);

    void start();

    ~Frontier();

   private:
    Server _server;
    PriorityQueue _pq;
    BloomFilter _filter;

    uint32_t _numUrls = 0;
    uint32_t _maxUrls = 0;

    int _batchSize;

    FrontierMessage _handleMessage(FrontierMessage msg);
};

#pragma once

#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>
#include <fcntl.h>
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
             std::string saveFile, int checkpointFrequency);


    void recoverFilter(std::string filePath);

    void start();

    ~Frontier();

   private:
    void _checkpoint();

    Server _server;
    PriorityQueue _pq;
    BloomFilter _filter;

    std::string _saveFileName;

    uint32_t _numUrls = 0;
    uint32_t _maxUrls = 0;

    int _batchSize;
    int _checkpointFrequency;

    FrontierMessage _handleMessage(FrontierMessage msg);
};

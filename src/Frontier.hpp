#pragma once

#include <fcntl.h>
#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "BloomFilter.hpp"
#include "FrontierInterface.hpp"
#include "GatewayServer.hpp"
#include "PriorityQueue.hpp"

using std::cout, std::endl;

class Frontier {
   public:
    Frontier(int port, int MAX_CLIENTS, uint32_t maxUrls, int batchSize,
             std::string seedList, std::string saveFile,
             int checkpointFrequency, int maxFrontierSize);

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
    int _lastCheckpoint = 0;

    std::string _seedList;

    FrontierMessage _handleMessage(FrontierMessage msg);
};

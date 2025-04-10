#include "Frontier.hpp"

Frontier::Frontier(int port, int maxClients, uint32_t maxUrls, int batchSize,
                   std::string seedList, std::string saveFileName,
                   int checkpointFrequency, int frontierCapacity)
    : _server(Server(port, maxClients)),
      _pq(PriorityQueue(frontierCapacity)),
      _filter(BloomFilter(maxUrls, 0.001)),
      _saveFileName(saveFileName),
      _maxUrls(maxUrls),
      _batchSize(batchSize),
      _checkpointFrequency(checkpointFrequency),
      _lastCheckpoint(0) {

    std::ifstream file(seedList);
    if (!file) {
        spdlog::error("Couldn't open {}", seedList);
        exit(EXIT_FAILURE);
    }

    std::string url;
    while (std::getline(file, url)) {
        if (!_filter.contains(url)) {
            _filter.insert(url);
            _pq.push(url);
        }
    }
    file.close();
}

Frontier::~Frontier() {}

void Frontier::_checkpoint() {
    spdlog::info("Checkpointing");
    std::ofstream saveFile(_saveFileName, std::ios::out | std::ios::trunc);

    // Write size of pq
    size_t pqSize = _pq.data.size();
    saveFile.write(reinterpret_cast<char*>(&pqSize), sizeof(pqSize));
    // Write pq
    for (const std::string& url : _pq.data) {
        size_t len = url.size();
        saveFile.write(reinterpret_cast<char*>(&len), sizeof(len));
        saveFile.write(url.data(), len);
    }

    // Write filter attributes
    saveFile.write(reinterpret_cast<char*>(&_filter.bits),
                   sizeof(_filter.bits));
    saveFile.write(reinterpret_cast<char*>(&_filter.numHashes),
                   sizeof(_filter.numHashes));

    // Write size of filter
    size_t filterSize = _filter.bloom.size();
    saveFile.write(reinterpret_cast<char*>(&filterSize), sizeof(filterSize));
    for (const bool& b : _filter.bloom) {
        saveFile.write(reinterpret_cast<const char*>(&b), sizeof(b));
    }
    saveFile.close();
}

void Frontier::recoverFilter(std::string filePath) {
    spdlog::info("Recovering pq and filter");
    std::ifstream saveFile(filePath, std::ios::binary);

    size_t pqSize = 0;
    saveFile.read(reinterpret_cast<char*>(&pqSize), sizeof(pqSize));
    if (pqSize <= 0) {
        spdlog::warn("Checkpoint file pq size is <= 0");
    }

    _pq.data.resize(pqSize);
    for (size_t i = 0; i < pqSize; ++i) {
        size_t len;
        saveFile.read(reinterpret_cast<char*>(&len), sizeof(len));
        _pq.data[i].resize(len);
        saveFile.read(_pq.data[i].data(), len);
    }

    size_t bits;
    size_t numHashes;
    saveFile.read(reinterpret_cast<char*>(&bits), sizeof(bits));
    saveFile.read(reinterpret_cast<char*>(&numHashes), sizeof(numHashes));

    size_t filterSize = 0;
    saveFile.read(reinterpret_cast<char*>(&filterSize), sizeof(filterSize));
    if (filterSize <= 0) {
        spdlog::warn("Filter pq size is <= 0");
    }

    _filter.bloom.resize(filterSize);
    for (size_t i = 0; i < filterSize; ++i) {
        char b;
        saveFile.read(&b, sizeof(b));
        _filter.bloom[i] = static_cast<bool>(b);
    }
}

void Frontier::start() {
    auto startTime = std::chrono::steady_clock::now();
    while (_numUrls < _maxUrls) {
        std::vector<Message> messages = _server.GetMessagesBlocking();
        for (auto m : messages) {
            spdlog::info("Request from {}:{}", m.senderIp, m.senderPort);

            FrontierMessage decodedMessage = FrontierInterface::Decode(m.msg);
            spdlog::info(decodedMessage.urls);
            FrontierMessage response = _handleMessage(decodedMessage);

            Message msg;
            msg.receiverSock = m.senderSock;
            msg.msg = FrontierInterface::Encode(response);

            _server.SendMessage(msg);
        }
        auto now = std::chrono::steady_clock::now();
        double elapsedSeconds =
            std::chrono::duration_cast<std::chrono::duration<double>>(now -
                                                                      startTime)
                .count();
        double elapsedMinutes = elapsedSeconds / 60.0;

        spdlog::info("Served {} out of {}", _numUrls, _maxUrls);
        spdlog::info("Frontier size: {}", _pq.size());

        if (elapsedSeconds > 0) {
            double urlsPerSecond = _numUrls / elapsedSeconds;
            spdlog::info("Elapsed time: {:.2f} minutes", elapsedMinutes);
            spdlog::info("{:.2f} URLs/second", urlsPerSecond);
        }

        if (_numUrls >= _lastCheckpoint + _checkpointFrequency) {
            _checkpoint();
            _lastCheckpoint += _numUrls;
        }
    }

    while (true) {
        std::vector<Message> messages = _server.GetMessagesBlocking();
        for (auto m : messages) {
            spdlog::info("Request from {}:{}. Sending END message back",
                         m.senderIp, m.senderPort);

            FrontierMessage endMessage{FrontierMessageType::END, {}};
            Message msg;
            msg.receiverSock = m.senderSock;
            msg.msg = FrontierInterface::Encode(endMessage);

            _server.SendMessage(msg);
        }
    }
}

std::string trim(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char c) {
        return std::isspace(c);
    });

    auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char c) {
                   return std::isspace(c);
               }).base();

    if (start >= end)
        return "";
    return std::string(start, end);
}

FrontierMessage Frontier::_handleMessage(FrontierMessage msg) {
    if (msg.type == FrontierMessageType::START) {
    } else if (msg.type == FrontierMessageType::ROBOTS) {
        // Add to robots.txt set
        spdlog::info("Robots URLS: {}", msg.urls);
        return FrontierMessage{FrontierMessageType::URLS, {}};
    }

    // Add to priority queue
    for (auto url : msg.urls) {
        std::string cleaned = trim(url);
        if (cleaned != "" && !_filter.contains(cleaned)) {
            _filter.insert(cleaned);
            _pq.push(cleaned);
        }
    }

    std::vector<std::string> urls = _pq.popN(_batchSize);
    _numUrls += urls.size();

    return FrontierMessage{FrontierMessageType::URLS, urls};
}

int main(int argc, char** argv) {
    argparse::ArgumentParser program("frontier");
    program.add_argument("-p", "--port")
        .default_value(8080)
        .help("Port to run server on")
        .scan<'i', int>();

    program.add_argument("-m", "--maxclients")
        .default_value(10)
        .help("Max number of clients")
        .scan<'i', int>();

    program.add_argument("-n", "--numurls")
        .default_value(1)
        .help("Number of urls to serve")
        .scan<'i', int>();

    program.add_argument("-b", "--batchsize")
        .default_value(4)
        .help("Number of urls to send in one response")
        .scan<'i', int>();

    program.add_argument("-s", "--savefile")
        .default_value("../frontier_save.txt")
        .help("Path to save file");

    program.add_argument("-l", "--seedlist")
        .required()
        .help("Path to seed list");

    program.add_argument("-f", "--frequency")
        .default_value(10000)
        .help("How frequently to checkpoint")
        .scan<'i', int>();

    program.add_argument("--recover")
        .default_value(false)
        .implicit_value(true)
        .help("Recover from the savefile");

    program.add_argument("-c", "--frontiercapacity")
        .default_value(10000)
        .scan<'i', int>();

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    int port = program.get<int>("-p");
    int maxClients = program.get<int>("-m");
    int numUrls = program.get<int>("-n");
    int batchSize = program.get<int>("-b");
    std::string saveFile = program.get<std::string>("-s");
    std::string seedList = program.get<std::string>("-l");
    int checkpointFrequency = program.get<int>("-f");
    bool recover = program.get<bool>("--recover");
    int frontierCapacity = program.get<int>("-c");

    spdlog::info("Port {}", port);
    spdlog::info("Max clients {}", maxClients);
    spdlog::info("Number of urls {}", numUrls);
    spdlog::info("Batch size {}", batchSize);
    spdlog::info("Save file path {}", saveFile);
    spdlog::info("Seed list file path {}", seedList);
    spdlog::info("Checkpoint frequency {}", checkpointFrequency);
    spdlog::info("PQ capacity {}", frontierCapacity);

    spdlog::info("======= Frontier Started =======");
    Frontier frontier(port, maxClients, numUrls, batchSize, seedList, saveFile,
                      checkpointFrequency, frontierCapacity);

    if (recover) {
        frontier.recoverFilter(saveFile);
    }

    frontier.start();
    spdlog::info("======= Frontier Finished =======");
}

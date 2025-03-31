#include "Frontier.hpp"

#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>

Frontier::Frontier(int port, int maxClients, uint32_t maxUrls, int batchSize, std::string seedList,
                   std::string saveFile)
    : _server(Server(port, maxClients)), _filter(BloomFilter(maxUrls, 0.001, saveFile)),
      _maxUrls(maxUrls), _batchSize(batchSize) {
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

Frontier::~Frontier() {
}

void Frontier::recoverFilter(const char* filePath) {
    return;
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
        double elapsedSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(now - startTime).count();
        double elapsedMinutes = elapsedSeconds / 60.0;

        spdlog::info("Served {} out of {}", _numUrls, _maxUrls);
        spdlog::info("Frontier size: {}", _pq.size());

        if (elapsedSeconds > 0) {
            double urlsPerSecond = _numUrls / elapsedSeconds;
            spdlog::info("Elapsed time: {:.2f} minutes", elapsedMinutes);
            spdlog::info("{:.2f} URLs/second", urlsPerSecond);
        }
    }

    while (true) {
        std::vector<Message> messages = _server.GetMessagesBlocking();
        for (auto m : messages) {
            spdlog::info("Request from {}:{}. Sending END message back", m.senderIp, m.senderPort);

            FrontierMessage endMessage{FrontierMessageType::END, {}};
            Message msg;
            msg.receiverSock = m.senderSock;
            msg.msg = FrontierInterface::Encode(endMessage);

            _server.SendMessage(msg);
        }
    }
}

std::string trim(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(),
                                  [](unsigned char c) { return std::isspace(c); });

    auto end = std::find_if_not(str.rbegin(), str.rend(),
                                [](unsigned char c) { return std::isspace(c); }).base();

    if (start >= end) return "";
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
        .default_value("./frontier_save.txt")
        .help("Path to save file");

    program.add_argument("-l", "--seedlist")
        .required()
        .help("Path to seed list");

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

    spdlog::info("Port {}", port);
    spdlog::info("Max clients {}", maxClients);
    spdlog::info("Number of urls {}", numUrls);
    spdlog::info("Batch size {}", batchSize);
    spdlog::info("Save file path {}", saveFile);
    spdlog::info("Seed list file path {}", seedList);

    spdlog::info("======= Frontier Started =======");
    Frontier frontier(port, maxClients, numUrls, batchSize, seedList, saveFile);

    if (argc == 7) {
        frontier.recoverFilter(argv[6]);
    }

    frontier.start();
    spdlog::info("======= Frontier Finished =======");
}

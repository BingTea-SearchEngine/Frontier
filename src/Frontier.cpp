#include "Frontier.hpp"
#include "FrontierInterface.hpp"

#include <fstream>
#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>

FrontierMessage receiveMessage(int clientSock) {
    uint32_t messageLength = 0;
    int bytesReceived = 0;
    // Get message size
    bytesReceived += recv(clientSock, &messageLength, sizeof(messageLength), 0);
    if (bytesReceived <= 0) {
        return FrontierMessage{MessageType::EMPTY};
    }

    messageLength = ntohl(messageLength);
    if (messageLength > 0) {
        std::string message(messageLength, '\0');
        if (recv(clientSock, message.data(), messageLength, MSG_WAITALL) <= 0) {
            return FrontierMessage{MessageType::EMPTY};
        }
        return FrontierInterface::Decode(message);
    }
    // Message length was 0, assume URLS
    return FrontierMessage{MessageType::URLS};
}

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
    while (_numUrls < _maxUrls) {
        std::vector<Message> messages = _server.GetMessagesBlocking();
        for (auto m : messages) {
            cout << m << endl;
        }
    }
}

int Frontier::_handleClient(int clientSock) {
    spdlog::info(">>> Request Start (Client: {})", clientSock);

    auto [type, receivedUrls] = receiveMessage(clientSock);
    if (type == MessageType::EMPTY) {
        spdlog::info("<<< Request End (Client: {})", clientSock);
        spdlog::info("==== Client disconnected: " + std::to_string(clientSock) +
                     " ====");
        close(clientSock);
        return 0;
    } else if (type == MessageType::ROBOTS) {
        // Add to robots.txt set
        spdlog::info("Robots URLS: {}", receivedUrls);
        // If robots, should expect another message with actual urls
        auto [newType, newReceivedUrls] = receiveMessage(clientSock);
        assert(newType == MessageType::URLS);
        receivedUrls = newReceivedUrls;
    }

    // Add to priority queue
    for (auto url : receivedUrls) {
        if (!_filter.contains(url)) {
            _filter.insert(url);
            _pq.push(url);
        }
    }

    std::vector<std::string> urls = _pq.popN(_batchSize);
    _numUrls += urls.size();

    // Send response back
    std::string response = FrontierInterface::Encode(FrontierMessage{MessageType::URLS, urls});
    spdlog::info("Sending {}", urls);
    uint32_t responseSize = htonl(response.size());
    send(clientSock, &responseSize, sizeof(responseSize), 0);
    send(clientSock, response.data(), response.size(), 0);
    spdlog::info("<<< Request End (Client: {})", clientSock);
    return 1;
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

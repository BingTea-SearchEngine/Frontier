#include "FrontierInterface.hpp"

#include <sstream>

std::string FrontierInterface::Encode(FrontierMessage message) {
    if (static_cast<int>(message.type) < 0 ||
        static_cast<int>(message.type) > 3) {
        throw std::runtime_error("Invalid Message Type header");
    }
    std::ostringstream oss;
    oss << MessageHeaders[static_cast<int>(message.type)] << '\0';

    // Encode urls
    {
        uint32_t numUrls = htonl(static_cast<uint32_t>(message.urls.size()));
        oss.write(reinterpret_cast<char*>(&numUrls), sizeof(numUrls));

        for (const auto& url : message.urls) {
            uint32_t len = htonl(static_cast<uint32_t>(url.size()));
            oss.write(reinterpret_cast<char*>(&len), sizeof(len));
            oss.write(url.data(), url.size());
        }
    }

    // Encode failed
    {
        uint32_t numFailed =
            htonl(static_cast<uint32_t>(message.failed.size()));
        oss.write(reinterpret_cast<char*>(&numFailed), sizeof(numFailed));

        // Write each failed URL
        for (const auto& f : message.failed) {
            uint32_t len = htonl(static_cast<uint32_t>(f.size()));
            oss.write(reinterpret_cast<char*>(&len), sizeof(len));
            oss.write(f.data(), f.size());
        }
    }

    return oss.str();
}

FrontierMessage FrontierInterface::Decode(const std::string& encoded) {
    std::istringstream iss(encoded);
    std::string header;
    std::getline(iss, header, '\0');

    FrontierMessageType messageType;
    if (header == "ROBOTS") {
        messageType = FrontierMessageType::ROBOTS;
    } else if (header == "URLS") {
        messageType = FrontierMessageType::URLS;
    } else if (header == "START") {
        messageType = FrontierMessageType::START;
    } else if (header == "END") {
        messageType = FrontierMessageType::END;
    } else {
        throw std::runtime_error("Invalid MessageType header" + header);
    }

    FrontierMessage message;
    message.type = messageType;

    // Read urls
    {
        uint32_t numUrlsN; 
        iss.read(reinterpret_cast<char*>(&numUrlsN), sizeof(numUrlsN));
        uint32_t numUrls = ntohl(numUrlsN);
        std::cout << numUrlsN << std::endl;

        // Read each URL
        message.urls.reserve(numUrls);
        for (uint32_t i = 0; i < numUrls; ++i) {
            uint32_t lenN;
            iss.read(reinterpret_cast<char*>(&lenN), sizeof(lenN));
            uint32_t len = ntohl(lenN);

            std::string url(len, '\0');
            iss.read(&url[0], len);
            message.urls.push_back(url);
        }
    }

    // Read failed
    {
        uint32_t numFailedN;
        iss.read(reinterpret_cast<char*>(&numFailedN), sizeof(numFailedN));
        uint32_t numFailed = ntohl(numFailedN);
        std::cout << numFailed << std::endl;

        // Read each failed URL
        message.failed.reserve(numFailed);
        for (uint32_t i = 0; i < numFailed; ++i) {
            uint32_t lenN;
            iss.read(reinterpret_cast<char*>(&lenN), sizeof(lenN));
            uint32_t len = ntohl(lenN);

            std::string f(len, '\0');
            iss.read(&f[0], len);
            message.failed.push_back(f);
        }
    }
    return message;
}

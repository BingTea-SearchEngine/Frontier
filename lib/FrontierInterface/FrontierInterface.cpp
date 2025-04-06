#include "FrontierInterface.hpp"

#include <sstream>

std::string FrontierInterface::Encode(FrontierMessage message) {
    if (static_cast<int>(message.type) < 0 ||
        static_cast<int>(message.type) > 3) {
        throw std::runtime_error("Invalid Message Type header");
    }
    std::ostringstream oss;
    oss << MessageHeaders[static_cast<int>(message.type)] << '\0';
    for (const auto& url : message.urls) {
        uint32_t len = htonl(static_cast<uint32_t>(url.size()));
        oss.write(reinterpret_cast<char*>(&len), sizeof(len));
        oss.write(url.data(), url.size());
    }
    return oss.str();
}

FrontierMessage FrontierInterface::Decode(const std::string& encoded) {
    std::vector<std::string> result;
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

    while (iss.peek() != EOF) {
        uint32_t len;
        iss.read(reinterpret_cast<char*>(&len), sizeof(len));
        len = ntohl(len);
        std::string url(len, '\0');
        iss.read(url.data(), len);
        result.push_back(url);
    }
    return {messageType, result};
}

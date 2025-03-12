#include "FrontierInterface.hpp"

#include <sstream>

std::string FrontierInterface::Encode(Message message) {
    std::ostringstream oss;
    oss << MessageHeaders[static_cast<int>(message.type)] << '\0';
    for (const auto& url : message.urls) {
        uint32_t len = htonl(static_cast<uint32_t>(url.size()));
        oss.write(reinterpret_cast<char*>(&len), sizeof(len));
        oss.write(url.data(), url.size());
    }
    return oss.str();
}

Message FrontierInterface::Decode(const std::string& encoded) {
    std::vector<std::string> result;
    std::istringstream iss(encoded);
    std::string header;
    std::getline(iss, header, '\0');

    MessageType messageType;
    if (header == "ROBOTS") {
        messageType = MessageType::ROBOTS;
    } else if (header == "URLS") {
        messageType = MessageType::URLS;
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

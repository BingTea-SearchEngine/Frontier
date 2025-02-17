#include "FrontierInterface.hpp"

#include <sstream>

std::string FrontierInterface::Encode(const std::vector<std::string>& urls) {
    std::ostringstream oss;
    for (const auto& url : urls) {
        uint32_t len = htonl(static_cast<uint32_t>(url.size()));
        oss.write(reinterpret_cast<char*>(&len), sizeof(len));
        oss.write(url.data(), url.size());
    }

    return oss.str();
}

std::vector<std::string> FrontierInterface::Decode(const std::string& encoded) {
    std::vector<std::string> result;
    std::istringstream iss(encoded);
    while (iss.peek() != EOF) {
        uint32_t len;
        iss.read(reinterpret_cast<char*>(&len), sizeof(len));
        len = ntohl(len);
        std::string url(len, '\0');
        iss.read(url.data(), len);
        result.push_back(url);
    }
    return result;
}

#include "FrontierInterface.hpp"

#include <sstream>

std::string FrontierInterface::Encode(const std::vector<std::string>& urls) {
    std::ostringstream oss;
    for (const auto& url : urls) {
        size_t len = url.size();
        oss.write(reinterpret_cast<char*>(&len), sizeof(len));
        oss.write(url.data(), len);
    }
    return oss.str();
}

std::vector<std::string> FrontierInterface::Decode(const std::string& encoded) {
    std::vector<std::string> result;
    std::istringstream iss(encoded);
    while (iss.peek() != EOF) {
        size_t len;
        iss.read(reinterpret_cast<char*>(&len), sizeof(len));
        std::string url(len, '\0');
        iss.read(url.data(), len);
        result.push_back(url);
    }
    return result;
}

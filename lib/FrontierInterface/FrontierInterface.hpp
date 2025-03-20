#pragma once

#include <string>
#include <vector>

enum class MessageType {
    ROBOTS = 0,
    URLS = 1,
    EMPTY = 2,
};

const std::string MessageHeaders[] = {
    "ROBOTS",
    "URLS"
};

struct FrontierMessage {
    MessageType type;
    std::vector<std::string> urls;
};

struct FrontierInterface {
    static std::string Encode(FrontierMessage message);

    static FrontierMessage Decode(const std::string& encoded);
};

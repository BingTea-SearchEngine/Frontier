#pragma once

#include <string>
#include <vector>

enum class MessageType {
    ROBOTS,
    URLS
};

const std::string MessageHeaders[] = {
    "ROBOTS",
    "URLS"
};

struct Message {
    MessageType type;
    std::vector<std::string> urls;
};

struct FrontierInterface {
    static std::string Encode(Message message);

    static Message Decode(const std::string& encoded);
};

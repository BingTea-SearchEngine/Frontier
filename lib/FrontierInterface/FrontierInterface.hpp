#pragma once

#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <vector>

enum class FrontierMessageType {
    START = 0,
    END = 1,
    URLS = 2,
    ROBOTS = 3,
};

const std::string MessageHeaders[] = {
    "START",
    "END",
    "URLS",
    "ROBOTS",
};

struct FrontierMessage {
    FrontierMessageType type;
    std::vector<std::string> urls;
    std::vector<std::string> failed;

    friend std::ostream& operator<<(std::ostream& os,
                                    const FrontierMessage& m) {
        os << "FrontierMessage { type=";

        switch (m.type) {
            case FrontierMessageType::ROBOTS:
                os << "ROBOTS";
                break;
            case FrontierMessageType::URLS:
                os << "URLS";
                break;
            case FrontierMessageType::START:
                os << "START";
                break;
            case FrontierMessageType::END:
                os << "START";
                break;
            default:
                os << "UNKNOWN";
                break;
        }

        os << ", urls=[";
        for (size_t i = 0; i < m.urls.size(); ++i) {
            os << "\"" << m.urls[i] << "\"";
            if (i != m.urls.size() - 1) {
                os << ", ";
            }
        }
        os << "] }";
        os << "], failed=[";
        for (size_t i = 0; i < m.failed.size(); ++i) {
            os << "\"" << m.failed[i] << "\"";
            if (i != m.failed.size() - 1) {
                os << ", ";
            }
        }
        os << "] }";
        return os;
    }
};

struct FrontierInterface {
    static std::string Encode(FrontierMessage message);

    static FrontierMessage Decode(const std::string& encoded);
};

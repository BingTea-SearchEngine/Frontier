#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

#include "FrontierInterface.hpp"

TEST(FrontierInterface, Basic) {
    std::vector<std::string> urls = {"google.com", "wikipedia.com",
                                     "https://github.com/wbjin"};

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);

    FrontierMessage robotsMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    encoded = FrontierInterface::Encode(robotsMessage);
    decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, decoded.type);
    EXPECT_EQ(decoded.urls, decoded.urls);
}

TEST(FrontierInterface, HandlesEmptyVector) {
    std::vector<std::string> urls = {};

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);

    FrontierMessage robotsMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    encoded = FrontierInterface::Encode(robotsMessage);
    decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, decoded.type);
    EXPECT_EQ(decoded.urls, decoded.urls);
}

TEST(FrontierInterface, HandlesSingleURL) {
    std::vector<std::string> urls = {"https://www.example.com"};

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);

    FrontierMessage robotsMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    encoded = FrontierInterface::Encode(robotsMessage);
    decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, decoded.type);
    EXPECT_EQ(decoded.urls, decoded.urls);
}

TEST(FrontierInterface, HandlesURLsWithDelimiters) {
    std::vector<std::string> urls = {"http://example.com?query=foo|bar",
                                     "https://test.com/path|segment"};

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);

    FrontierMessage robotsMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    encoded = FrontierInterface::Encode(robotsMessage);
    decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, decoded.type);
    EXPECT_EQ(decoded.urls, decoded.urls);
}

TEST(FrontierInterface, HandlesLongURLs) {
    std::vector<std::string> urls = {
        "https://example.com/" + std::string(1000, 'a'),
        "https://long-url.com/" + std::string(2000, 'b')};

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);

    FrontierMessage robotsMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    encoded = FrontierInterface::Encode(robotsMessage);
    decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, decoded.type);
    EXPECT_EQ(decoded.urls, decoded.urls);
}

TEST(FrontierInterface, HandlesSpecialCharacters) {
    std::vector<std::string> urls = {"https://exämple.com", "https://你好.com",
                                     "https://test.com/space%20url",
                                     "https://[::1]:8080"};

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);

    FrontierMessage robotsMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    encoded = FrontierInterface::Encode(robotsMessage);
    decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, decoded.type);
    EXPECT_EQ(decoded.urls, decoded.urls);
}

TEST(FrontierInterface, HandlesEmptyStrings) {
    std::vector<std::string> urls = {"", "https://example.com", ""};
}

TEST(FrontierInterface, HandlesMixedCases) {
    std::vector<std::string> urls = {"http://lowercase.com",
                                     "HTTPS://UPPERCASE.COM",
                                     "Http://MixedCase.com"};

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);

    FrontierMessage robotsMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    encoded = FrontierInterface::Encode(robotsMessage);
    decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, decoded.type);
    EXPECT_EQ(decoded.urls, decoded.urls);
}

TEST(FrontierInterface, HandlesBinaryDataInURL) {
    std::vector<std::string> urls = {"https://example.com/\x01\x02\x03\x04",
                                     "https://binarydata.com/\xff\xfe\xfd"};

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);

    FrontierMessage robotsMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    encoded = FrontierInterface::Encode(robotsMessage);
    decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, decoded.type);
    EXPECT_EQ(decoded.urls, decoded.urls);
}

TEST(FrontierInterface, HandlesMassiveNumberOfURLs) {
    std::vector<std::string> urls;
    for (int i = 0; i < 10000; ++i) {
        urls.push_back("https://example.com/page" + std::to_string(i));
    }

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);

    FrontierMessage robotsMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    encoded = FrontierInterface::Encode(robotsMessage);
    decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, decoded.type);
    EXPECT_EQ(decoded.urls, decoded.urls);
}

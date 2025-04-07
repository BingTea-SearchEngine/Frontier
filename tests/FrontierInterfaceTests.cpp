#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

#include "FrontierInterface.hpp"

TEST(FrontierInterface, Basic) {
    std::vector<std::string> urls = {"google.com", "wikipedia.com",
                                     "https://github.com/wbjin"};
    std::vector<std::string> failed = {"failed1.com", "failed2.net"};

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls, failed};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);
    EXPECT_EQ(decoded.failed, urlMessage.failed);

    FrontierMessage robotsMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    encoded = FrontierInterface::Encode(robotsMessage);
    decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, decoded.type);
    EXPECT_EQ(decoded.urls, decoded.urls);
}

TEST(FrontierInterface, HandlesEmptyVector) {
    std::vector<std::string> urls = {};
    std::vector<std::string> failed = {};

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls, failed};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);
    EXPECT_EQ(decoded.failed, urlMessage.failed);

    FrontierMessage robotsMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    encoded = FrontierInterface::Encode(robotsMessage);
    decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, decoded.type);
    EXPECT_EQ(decoded.urls, decoded.urls);
}

TEST(FrontierInterface, HandlesSingleURL) {
    std::vector<std::string> urls = {"https://www.example.com"};
    std::vector<std::string> failed = {"https://failed.example.org"};

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls, failed};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);
    EXPECT_EQ(decoded.failed, urlMessage.failed);

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
    std::vector<std::string> failed = {
        "http://failed.com/foo|bar",
        "https://failed.com/another|one"
    };

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls, failed};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);
    EXPECT_EQ(decoded.failed, urlMessage.failed);

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
    std::vector<std::string> failed = {
        "https://failedurl.com/" + std::string(500, 'z')
    };


    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls, failed};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);
    EXPECT_EQ(decoded.failed, urlMessage.failed);

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
    std::vector<std::string> failed = {
        "https://badäsite.net",
        "https://另一个失败.com"
    };

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls, failed};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);
    EXPECT_EQ(decoded.failed, urlMessage.failed);

    FrontierMessage robotsMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    encoded = FrontierInterface::Encode(robotsMessage);
    decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, decoded.type);
    EXPECT_EQ(decoded.urls, decoded.urls);
}

TEST(FrontierInterface, HandlesEmptyStrings) {
    std::vector<std::string> urls = {"", "https://example.com", ""};
    std::vector<std::string> failed = {""};

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls, failed};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);
    EXPECT_EQ(decoded.failed, urlMessage.failed);
}

TEST(FrontierInterface, HandlesMixedCases) {
    std::vector<std::string> urls = {"http://lowercase.com",
                                     "HTTPS://UPPERCASE.COM",
                                     "Http://MixedCase.com"};
    std::vector<std::string> failed = {
        "http://FAIledCASE.com"
    };

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls, failed};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);
    EXPECT_EQ(decoded.failed, urlMessage.failed);

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
    std::vector<std::string> failed = {
        "https://fail.com/\x05\x06",
        "https://failedbinary/\xaa\xbb\xcc"
    };

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls, failed};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);
    EXPECT_EQ(decoded.failed, urlMessage.failed);

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
    std::vector<std::string> failed;
    for (int i = 0; i < 2000; ++i) {
        failed.push_back("https://fail.com/page" + std::to_string(i));
    }

    FrontierMessage urlMessage =
        FrontierMessage{FrontierMessageType::URLS, urls, failed};
    std::string encoded = FrontierInterface::Encode(urlMessage);
    FrontierMessage decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, urlMessage.type);
    EXPECT_EQ(decoded.urls, urlMessage.urls);
    EXPECT_EQ(decoded.failed, urlMessage.failed);


    FrontierMessage robotsMessage =
        FrontierMessage{FrontierMessageType::URLS, urls};
    encoded = FrontierInterface::Encode(robotsMessage);
    decoded = FrontierInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, decoded.type);
    EXPECT_EQ(decoded.urls, decoded.urls);
}

#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <iostream>

#include "FrontierInterface.hpp"

TEST(FrontierInterface, Basic) {
    std::vector<std::string> urls = {"google.com", "wikipedia.com", "https://github.com/wbjin"};
    
    std::string encoded = FrontierInterface::Encode(urls);
    
    std::cout << encoded << std::endl;
    EXPECT_EQ(FrontierInterface::Decode(encoded), urls);
}

TEST(FrontierInterface, HandlesEmptyVector) {
    std::vector<std::string> urls = {};
    
    std::string encoded = FrontierInterface::Encode(urls);
    
    EXPECT_EQ(FrontierInterface::Decode(encoded), urls);
}

TEST(FrontierInterface, HandlesSingleURL) {
    std::vector<std::string> urls = {"https://www.example.com"};
    
    std::string encoded = FrontierInterface::Encode(urls);
    
    EXPECT_EQ(FrontierInterface::Decode(encoded), urls);
}

TEST(FrontierInterface, HandlesURLsWithDelimiters) {
    std::vector<std::string> urls = {"http://example.com?query=foo|bar", "https://test.com/path|segment"};
    
    std::string encoded = FrontierInterface::Encode(urls);
    
    EXPECT_EQ(FrontierInterface::Decode(encoded), urls);
}

TEST(FrontierInterface, HandlesLongURLs) {
    std::vector<std::string> urls = {
        "https://example.com/" + std::string(1000, 'a'),
        "https://long-url.com/" + std::string(2000, 'b')
    };
    
    std::string encoded = FrontierInterface::Encode(urls);
    
    EXPECT_EQ(FrontierInterface::Decode(encoded), urls);
}

TEST(FrontierInterface, HandlesSpecialCharacters) {
    std::vector<std::string> urls = {
        "https://exämple.com", 
        "https://你好.com", 
        "https://test.com/space%20url", 
        "https://[::1]:8080"
    };
    
    std::string encoded = FrontierInterface::Encode(urls);
    
    EXPECT_EQ(FrontierInterface::Decode(encoded), urls);
}

TEST(FrontierInterface, HandlesEmptyStrings) {
    std::vector<std::string> urls = {"", "https://example.com", ""};
    
    std::string encoded = FrontierInterface::Encode(urls);
    
    EXPECT_EQ(FrontierInterface::Decode(encoded), urls);
}

TEST(FrontierInterface, HandlesMixedCases) {
    std::vector<std::string> urls = {
        "http://lowercase.com",
        "HTTPS://UPPERCASE.COM",
        "Http://MixedCase.com"
    };
    
    std::string encoded = FrontierInterface::Encode(urls);
    
    EXPECT_EQ(FrontierInterface::Decode(encoded), urls);
}

TEST(FrontierInterface, HandlesBinaryDataInURL) {
    std::vector<std::string> urls = {
        "https://example.com/\x01\x02\x03\x04",
        "https://binarydata.com/\xff\xfe\xfd"
    };
    
    std::string encoded = FrontierInterface::Encode(urls);
    
    EXPECT_EQ(FrontierInterface::Decode(encoded), urls);
}

TEST(FrontierInterface, HandlesMassiveNumberOfURLs) {
    std::vector<std::string> urls;
    for (int i = 0; i < 10000; ++i) {
        urls.push_back("https://example.com/page" + std::to_string(i));
    }
    
    std::string encoded = FrontierInterface::Encode(urls);
    
    EXPECT_EQ(FrontierInterface::Decode(encoded), urls);
}


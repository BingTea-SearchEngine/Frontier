#pragma once

#include <string>
#include <vector>

struct FrontierInterface {
    static std::string Encode(const std::vector<std::string>& urls);

    static std::vector<std::string> Decode(const std::string& encoded);
};

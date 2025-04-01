#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class PriorityQueue {
public:
    explicit PriorityQueue(size_t reserveCapacity = 100);

    void push(std::string elm);
    std::string pop();

    std::vector<std::string> popN(size_t N);

    int getPriorityForTld(const std::string &tld) const;

    size_t size();

private:
    friend class Frontier;

    std::vector<std::string> data;

    std::unordered_map<std::string, int> priorityMap;

    int computePriority(const std::string &url);

    void adjustPriority(const std::string &url);

    bool compareURL(const std::string &a, const std::string &b);

    void siftUp(size_t i);
    void siftDown(size_t i);
};

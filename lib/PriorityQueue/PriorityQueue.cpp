#include "PriorityQueue.hpp"
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <string>
#include <unordered_map>

std::unordered_map<std::string, int> priorityMap = {
    {".edu", 5},
    {".gov", 4},
    {".org", 3},
    {".com", 2},
    {".net", 1}
};

int computePriority(const std::string &url) {
    size_t pos = url.rfind('.');
    if (pos != std::string::npos) {
        std::string tld = url.substr(pos);
        if (priorityMap.find(tld) == priorityMap.end()) {
            priorityMap[tld] = 0;
        }
        return priorityMap[tld];
    }
    return 0;
}

void adjustPriority(const std::string &url) {
    size_t pos = url.rfind('.');
    if (pos != std::string::npos) {
        std::string tld = url.substr(pos);
        if (priorityMap.find(tld) == priorityMap.end()) {
            priorityMap[tld] = 0;
        }
        ++priorityMap[tld];
    }
}

bool compareURL(const std::string &a, const std::string &b) {
    int pa = computePriority(a);
    int pb = computePriority(b);
    if (pa == pb)
        return a < b;
    return pa > pb;
}

PriorityQueue::PriorityQueue() {}

void PriorityQueue::push(std::string elm) {
    data.push_back(std::move(elm));
    siftUp(data.size() - 1);
}

std::string PriorityQueue::pop() {
    if (data.empty())
        throw std::runtime_error("PriorityQueue is empty");

    std::string top = std::move(data[0]);

    adjustPriority(top);

    data[0] = std::move(data.back());
    data.pop_back();

    if (!data.empty())
        siftDown(0);

    return top;
}

std::vector<std::string> PriorityQueue::popN(size_t N) {
    std::vector<std::string> result;
    result.reserve(N);
    for (size_t i = 0; i < N && !data.empty(); ++i)
        result.push_back(pop());
    return result;
}

void PriorityQueue::siftUp(size_t i) {
    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (compareURL(data[i], data[parent])) {
            std::swap(data[i], data[parent]);
            i = parent;
        } else {
            break;
        }
    }
}

void PriorityQueue::siftDown(size_t i) {
    size_t n = data.size();
    while (true) {
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;
        size_t best = i;

        if (left < n && compareURL(data[left], data[best]))
            best = left;
        if (right < n && compareURL(data[right], data[best]))
            best = right;

        if (best != i) {
            std::swap(data[i], data[best]);
            i = best;
        } else {
            break;
        }
    }
}

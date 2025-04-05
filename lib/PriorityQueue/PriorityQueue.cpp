#include "PriorityQueue.hpp"
#include <algorithm>
#include <utility>

// Constructor: reserves capacity and initializes the priority map.
PriorityQueue::PriorityQueue(size_t reserveCapacity)
    : maxCapacity(reserveCapacity) // store the max capacity
{
    data.reserve(reserveCapacity);
    // Default priorities for known TLDs.
    priorityMap = {
        {".edu", 5},
        {".gov", 4},
        {".org", 3},
        {".com", 2},
        {".net", 1}
    };
}

// Computes the priority of a URL based on its top-level domain.
int PriorityQueue::computePriority(const std::string &url) {
    size_t pos = url.rfind('.');
    if (pos != std::string::npos) {
        std::string tld = url.substr(pos);
        if (priorityMap.find(tld) == priorityMap.end()) {
            // New TLDs start with a default priority of 0.
            priorityMap[tld] = 0;
        }
        return priorityMap[tld];
    }
    return 0;
}

// Adjusts the priority for the URL's TLD (e.g., after it is popped).
void PriorityQueue::adjustPriority(const std::string &url) {
    size_t pos = url.rfind('.');
    if (pos != std::string::npos) {
        std::string tld = url.substr(pos);
        if (priorityMap.find(tld) == priorityMap.end()) {
            priorityMap[tld] = 0;
        }
        ++priorityMap[tld];
    }
}

// Returns true if URL 'a' has a higher priority than URL 'b'.
// If priorities are equal, it compares them lexicographically.
bool PriorityQueue::compareURL(const std::string &a, const std::string &b) {
    int pa = computePriority(a);
    int pb = computePriority(b);
    if (pa == pb)
        return a < b;
    return pa > pb;
}

void PriorityQueue::push(std::string elm) {
    data.push_back(std::move(elm));
    siftUp(data.size() - 1);

    if (data.size() > maxCapacity) {
        data.pop_back();
    }
}

std::string PriorityQueue::pop() {
    if (data.empty())
        throw std::runtime_error("PriorityQueue is empty");

    std::string top = std::move(data[0]);

    // Adjust the priority of the popped URL.
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

size_t PriorityQueue::size() {
    return data.size();
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

// New public accessor: returns the current priority for the given TLD.
int PriorityQueue::getPriorityForTld(const std::string &tld) const {
    auto it = priorityMap.find(tld);
    return (it != priorityMap.end()) ? it->second : 0;
}

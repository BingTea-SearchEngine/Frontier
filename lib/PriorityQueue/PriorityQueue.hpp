// PriorityQueue.hpp
#pragma once

#include <vector>
#include <string>

class PriorityQueue {
public:
    PriorityQueue();

    void push(std::string elm);

    std::string pop();

    std::vector<std::string> popN(size_t N);

private:
    std::vector<std::string> data;

    void siftUp(size_t i);
    void siftDown(size_t i);
};


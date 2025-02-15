#include "PriorityQueue.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>  // for std::move and std::swap

PriorityQueue::PriorityQueue() {}

void PriorityQueue::push(std::string elm) {
    // Add the element to the end and restore the heap property.
    data.push_back(std::move(elm));
    siftUp(data.size() - 1);
}

std::string PriorityQueue::pop() {
    if (data.empty())
        throw std::runtime_error("PriorityQueue is empty");

    // Retrieve the highest priority (smallest) element.
    std::string top = std::move(data[0]);

    // Replace the top with the last element.
    data[0] = std::move(data.back());
    data.pop_back();

    // Restore the heap property.
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
        // If the current element is less than its parent, swap them.
        if (data[i] < data[parent]) {
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
        size_t smallest = i;

        // Find the smallest among the current element and its children.
        if (left < n && data[left] < data[smallest])
            smallest = left;
        if (right < n && data[right] < data[smallest])
            smallest = right;

        // If the smallest is not the current element, swap and continue.
        if (smallest != i) {
            std::swap(data[i], data[smallest]);
            i = smallest;
        } else {
            break;
        }
    }
}

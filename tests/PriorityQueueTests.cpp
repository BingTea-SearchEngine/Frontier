// PriorityQueue_test.cpp
#include <stdexcept>
#include <string>
#include <vector>
#include "PriorityQueue.hpp"
#include "gtest/gtest.h"

// Test that a single push/pop works correctly.
TEST(PriorityQueueTest, SinglePushPop) {
    PriorityQueue pq;
    pq.push("hello");
    std::string result = pq.pop();
    EXPECT_EQ(result, "hello");
}

// Test that multiple pushes and pops return strings in lexicographical order.
// Since the priority queue is a min‑heap, the smallest (lexicographically first)
// string should be popped first.
TEST(PriorityQueueTest, MultiplePushPop) {
    PriorityQueue pq;
    pq.push("zebra");
    pq.push("apple");
    pq.push("orange");
    pq.push("banana");

    // Lexicographical order: "apple" < "banana" < "orange" < "zebra"
    EXPECT_EQ(pq.pop(), "apple");
    EXPECT_EQ(pq.pop(), "banana");
    EXPECT_EQ(pq.pop(), "orange");
    EXPECT_EQ(pq.pop(), "zebra");
}

// Test that popN correctly returns a vector containing up to N elements in sorted order.
TEST(PriorityQueueTest, PopNTest) {
    PriorityQueue pq;
    pq.push("zebra");
    pq.push("apple");
    pq.push("orange");
    pq.push("banana");
    pq.push("cherry");

    // Expected lexicographical order: "apple", "banana", "cherry", "orange", "zebra"
    std::vector<std::string> popped = pq.popN(3);
    std::vector<std::string> expected = {"apple", "banana", "cherry"};

    EXPECT_EQ(popped, expected);
}

// Test that calling pop on an empty queue throws a runtime_error.
TEST(PriorityQueueTest, EmptyPopThrows) {
    PriorityQueue pq;
    EXPECT_THROW(pq.pop(), std::runtime_error);
}

// Optional: main() to run all tests.
// You can omit this if linking against gtest_main.
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

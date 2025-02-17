// PriorityQueue_test.cpp
#include <stdexcept>
#include <string>
#include <vector>
#include "PriorityQueue.hpp"
#include "gtest/gtest.h"
#include "PriorityQueue.hpp"
#include <vector>
#include <string>

// Test fixture: each test gets its own instance of PriorityQueue,
// so the internal priority map is automatically set to its default state.
class PriorityQueueTest : public ::testing::Test {
protected:
    // You can initialize common objects here if needed.
};

// Test that verifies the basic ordering based on the initial priorities.
TEST_F(PriorityQueueTest, BasicOrdering) {
    PriorityQueue pq;
    // Insert URLs with known TLDs.
    pq.push("a.edu");  // priority 5
    pq.push("b.com");  // priority 2
    pq.push("c.gov");  // priority 4
    pq.push("d.net");  // priority 1
    pq.push("e.org");  // priority 3

    // Expected ordering (highest priority first):
    // "a.edu" (5), "c.gov" (4), "e.org" (3), "b.com" (2), "d.net" (1)
    EXPECT_EQ(pq.pop(), "a.edu");
    EXPECT_EQ(pq.pop(), "c.gov");
    EXPECT_EQ(pq.pop(), "e.org");
    EXPECT_EQ(pq.pop(), "b.com");
    EXPECT_EQ(pq.pop(), "d.net");
}

// Test dynamic adjustment: after popping a URL, the TLD's priority should be incremented.
TEST_F(PriorityQueueTest, DynamicAdjustment) {
    PriorityQueue pq;
    // Push two URLs with the same TLD (.com), which initially has a priority of 2.
    pq.push("a.com");  
    pq.push("b.com");  

    // Alphabetical order breaks ties: "a.com" should be popped first.
    EXPECT_EQ(pq.pop(), "a.com");
    // After popping, the .com priority should have been incremented from 2 to 3.
    EXPECT_EQ(pq.getPriorityForTld(".com"), 3);

    // Push another URL with .com.
    pq.push("c.com");

    // Now both "b.com" and "c.com" have .com with priority 3.
    // Alphabetical order should put "b.com" before "c.com".
    EXPECT_EQ(pq.pop(), "b.com");
    EXPECT_EQ(pq.pop(), "c.com");
}

// Test insertion of a new TLD and that its priority adjusts after being popped.
TEST_F(PriorityQueueTest, NewTldInsertionAndAdjustment) {
    PriorityQueue pq;
    // Use a new TLD (.xyz) that is not in the initial map.
    pq.push("a.xyz");
    
    // Before popping, the computed priority for .xyz should be the default (0).
    EXPECT_EQ(pq.getPriorityForTld(".xyz"), 0);
    
    // Pop the URL, which adjusts the priority.
    EXPECT_EQ(pq.pop(), "a.xyz");
    
    // After popping, the priority for .xyz should have incremented by 1.
    EXPECT_EQ(pq.getPriorityForTld(".xyz"), 1);

    // Push another URL with .xyz and one with .edu.
    pq.push("b.edu");   // priority 5
    pq.push("c.xyz");   // now .xyz priority is 1

    // "b.edu" should have higher priority than "c.xyz".
    EXPECT_EQ(pq.pop(), "b.edu");
    EXPECT_EQ(pq.pop(), "c.xyz");
}

// Test that popN returns the expected ordering of multiple elements.
TEST_F(PriorityQueueTest, PopNTest) {
    PriorityQueue pq;
    pq.push("a.edu");  // 5
    pq.push("b.gov");  // 4
    pq.push("c.org");  // 3
    pq.push("d.com");  // 2

    // Expected order: "a.edu", "b.gov", "c.org", "d.com"
    std::vector<std::string> expected = {"a.edu", "b.gov", "c.org", "d.com"};
    std::vector<std::string> result = pq.popN(4);
    EXPECT_EQ(result, expected);
}

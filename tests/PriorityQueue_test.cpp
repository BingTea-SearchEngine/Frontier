#include "gtest/gtest.h"
#include "PriorityQueue.hpp"  // Include your priority queue header.
#include <vector>
#include <string>
#include <unordered_map>

// These extern declarations assume that your PriorityQueue.cpp exposes these functions and the global map.
extern std::unordered_map<std::string, int> priorityMap;
extern int computePriority(const std::string &url);
extern void adjustPriority(const std::string &url);

// Helper function to reset the global priority map to its default state.
// This is useful for ensuring that each test starts with the same initial conditions.
void resetPriorityMap() {
    priorityMap.clear();
    priorityMap[".edu"] = 5;
    priorityMap[".gov"] = 4;
    priorityMap[".org"] = 3;
    priorityMap[".com"] = 2;
    priorityMap[".net"] = 1;
}

// Test fixture to reset global state before each test.
class PriorityQueueTest : public ::testing::Test {
protected:
    void SetUp() override {
        resetPriorityMap();
    }
};

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

TEST_F(PriorityQueueTest, DynamicAdjustment) {
    PriorityQueue pq;
    // Push two URLs with the same TLD (.com) that initially have priority 2.
    pq.push("a.com");  
    pq.push("b.com");  

    // Alphabetical order breaks ties, so "a.com" should pop first.
    EXPECT_EQ(pq.pop(), "a.com");
    // Dynamic adjustment increments .com's priority:
    // Initially ".com" was 2, now it should be 3.
    EXPECT_EQ(computePriority("dummy.com"), 3);  // using any URL with .com

    // Push another URL with .com.
    pq.push("c.com");

    // Now both "b.com" and "c.com" have .com with priority 3.
    // Alphabetical order should put "b.com" before "c.com".
    EXPECT_EQ(pq.pop(), "b.com");
    EXPECT_EQ(pq.pop(), "c.com");
}

TEST_F(PriorityQueueTest, NewTldInsertionAndAdjustment) {
    PriorityQueue pq;
    // Use a new TLD (.xyz) that is not in the initial map.
    pq.push("a.xyz");
    
    // Before popping, the computed priority for .xyz should be the default (0),
    // and the new TLD should be added automatically.
    EXPECT_EQ(computePriority("a.xyz"), 0);
    
    // Pop the URL, which adjusts the priority.
    EXPECT_EQ(pq.pop(), "a.xyz");
    
    // After popping, the priority for .xyz should have incremented by 1.
    EXPECT_EQ(computePriority("a.xyz"), 1);

    // Push another URL with .xyz and one with .edu.
    pq.push("b.edu");   // priority 5
    pq.push("c.xyz");   // now priority 1 for .xyz

    // "b.edu" should have higher priority than "c.xyz".
    EXPECT_EQ(pq.pop(), "b.edu");
    EXPECT_EQ(pq.pop(), "c.xyz");
}

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

#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <iostream>

#include <openssl/md5.h>

class BloomFilter {
   public:
    BloomFilter(int num_objects, double false_positive_rate)
        : bits(-(num_objects * std::log(false_positive_rate)) /
               std::pow((std::log(2)), 2)),
          numHashes(static_cast<float>(bits) / num_objects * std::log(2)),
          bloom(bits) {
        // Determine the size of bits of our data vector, and resize.
        std::cout << bits << std::endl;

        // Determine number of hash functions to use.
    }

    void insert(const std::string& s) {
        // Hash the string into two unique hashes.
        std::pair<uint64_t, uint64_t> sHash = hash(s);
        uint64_t lHash = sHash.first;
        uint64_t rHash = sHash.second;

        // Use double hashing to get unique bit, and repeat for each hash function.
        for (int i = 0; i < numHashes; ++i) {
            bloom[doubleHash(lHash, rHash, i)] = 1;
        }
    }

    bool contains(const std::string& s) {
        // Hash the string into two unqiue hashes.
        std::pair<uint64_t, uint64_t> sHash = hash(s);
        uint64_t lHash = sHash.first;
        uint64_t rHash = sHash.second;
        // Use double hashing to get unique bit, and repeat for each hash function.
        // If bit is false, we know for certain this unique string has not been inserted.

        // If all bits were true, the string is likely inserted, but false positive is possible.

        for (int i = 0; i < numHashes; ++i) {
            if (!bloom[doubleHash(lHash, rHash, i)])
                return false;
        }

        // This line is for compiling, replace this with own code.
        return true;
    }

   private:
    // Add any private member variables that may be neccessary.
    friend class Frontier;

    size_t bits;
    size_t numHashes;
    std::vector<bool> bloom;

    size_t doubleHash(uint64_t lHash, uint64_t rHash, size_t itr) {
        return (lHash + itr * rHash) % bits;
    }

    std::pair<uint64_t, uint64_t> hash(const std::string& datum) {
        //Use MD5 to hash the string into one 128 bit hash, and split into 2 hashes.
        unsigned char result[MD5_DIGEST_LENGTH];
        MD5(reinterpret_cast<const unsigned char*>(datum.c_str()), datum.size(),
            result);
        uint64_t lHash = 0, rHash = 0;
        memcpy(&lHash, result, 8);
        memcpy(&rHash, result + 8, 8);
        //This line is for compiling, replace this with own code.
        return {lHash, rHash};
    }
};

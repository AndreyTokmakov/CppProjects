/**============================================================================
Name        : Trie.h
Created on  : 15.05.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Trie
============================================================================**/

#include "Trie.h"

#include <iostream>
#include <array>
#include <unordered_set>
#include <vector>
#include <memory>
#include <chrono>
#include <random>

namespace Trie
{
    class Trie
    {
        struct TrieNode {
            std::array<std::unique_ptr<TrieNode>, 26> children{};
            bool isEndOfWord{false};
        };

        std::unique_ptr<TrieNode> root{};


    public:
        Trie() : root{std::make_unique<TrieNode>()} {
        }

        void insert(std::string_view word)
        {
            TrieNode *currNode = root.get();
            for (const char c: word)
            {
                const int index = c - 'a';
                if (nullptr == currNode->children[index]) {
                    currNode->children[index] = std::make_unique<TrieNode>();
                }
                currNode = currNode->children[index].get();
            }
            currNode->isEndOfWord = true;
        }

        [[nodiscard]]
        bool search(const std::string& word) const
        {
            TrieNode *currNode = root.get();
            for (const char c: word)
            {
                const int index = c - 'a';
                if (nullptr == currNode->children[index])
                    return false;
                currNode = currNode->children[index].get();
            }
            return currNode->isEndOfWord;
        }

        /*
        [[nodiscard]]
        bool search(std::string_view word) const
        {
            TrieNode *currNode = root.get();
            for (const char c: word)
            {
                const int index = c - 'a';
                if (nullptr == currNode->children[index])
                    return false;
                currNode = currNode->children[index].get();
            }
            return currNode->isEndOfWord;
        }
         */
    };
}

namespace Trie::Tests
{
    std::string randomString(size_t size = 16)
    {
        std::random_device rd{};
        std::mt19937 generator = std::mt19937 {rd()};
        auto ud = std::uniform_int_distribution<> {(int)'a', (int)'z'};

        std::string str;
        str.reserve(size);
        while (size-- > 0)
            str.push_back(static_cast<char>(ud(generator)));
        return str;
    }

    void test()
    {
        Trie t;

        const std::string text { "12345" };

        std::cout << t.search(text) << std::endl;
        t.insert(text);
        std::cout << t.search(text) << std::endl;
    }

    void PerformanceTests()
    {
        constexpr size_t samplesCount { 1'000'000 }, strLen { 8 }, testsCount { 1 };
        std::vector<std::string> samples;
        samples.reserve(samplesCount);

        for (size_t idx = 0; idx < samplesCount; ++idx) {
            samples.push_back(randomString(strLen));
        }

        Trie trie;
        std::unordered_set<std::string> set;
        for (const std::string& str: samples){
            set.insert(str);
            trie.insert(str);
        }

        {
            auto start = std::chrono::high_resolution_clock::now();

            for (size_t idx = 0; idx < testsCount; ++idx) {
                for (const std::string& str: samples)
                {
                    if (set.find(str) == set.end())
                    {
                        std::cout << "ERROR\n";
                    }
                }
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "Result: " << duration << " microseconds" << std::endl;
        }

        {
            auto start = std::chrono::high_resolution_clock::now();

            for (size_t idx = 0; idx < testsCount; ++idx) {
                for (const std::string& str: samples) {
                    if (!trie.search(str))
                    {
                        std::cout << "ERROR\n";
                    }
                }
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            std::cout << "Result: " << duration << " microseconds" << std::endl;
        }
    }
}


void Trie::TestAll()
{
    // Tests::test();
    Tests::PerformanceTests();
}

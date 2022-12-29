//============================================================================
// Name        : BlockChain.cpp
// Created on  : 15.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : BlockChain C++ project
//============================================================================

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <tuple>

#include <thread>
#include <future>

#include <mutex>
#include <atomic>

#include <cmath>
#include <numeric>
#include <numbers>
#include <ostream>

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <array>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <span>

#include <concepts>
#include "cryptography/sha256_Original.h"
#include "cryptography/Sha256.h"


class Block {
private:
    uint32_t index { 0 };
    int64_t nonce { -1 };
    std::string data;
    std::string hash;
    std::string sPrevHash;
    time_t time;


public:
    Block(uint32_t index, std::string sDataIn): index {index}, data { std::move(sDataIn)} {

    }

    void MineBlock(uint32_t nDifficulty);

    std::string getHash();
    std::string calculateHash() const;
};


class Blockchain {
private:
    uint32_t difficulty;
    std::vector<Block> chain;

public:
    Blockchain();

    // TODO: to list??
    void AddBlock(Block bNew);
    Block _GetLastBlock() const;
};


namespace Sha256Tests {

    void Tests() {
        std::string input = "1232323";
        std::string expected = "1092ab2771a1b8d2d88fad71b09d03875c6df0c429ab02faf522524d44cbe4a8";

        std::cout << std::boolalpha << (expected == sha256_Orig(input)) << std::endl;
        std::cout << std::boolalpha << (expected == toSha256(input)) << std::endl;
    }
}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    Sha256Tests::Tests();

    return EXIT_SUCCESS;
}


//============================================================================
// Name        : FuzzingExperiments.cpp
// Created on  : 03.12.2022
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : FuzzingExperiments C++ project
//============================================================================

#include <optional>
#include <iostream>
#include <string>
#include <string_view>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <tuple>
#include <exception>
#include <random>
#include <thread>
#include <future>
#include <mutex>

#include <numeric>

#include <utility>
#include <vector>
#include <any>
#include <list>
#include <forward_list>
#include <deque>
#include <map>
#include <algorithm>
#include <array>
#include <version>
#include <concepts>
#include <unordered_set>

bool FuzzMe(const uint8_t *Data, size_t DataSize) {
    return DataSize >= 3 &&
           Data[0] == 'F' &&
           Data[1] == 'U' &&
           Data[2] == 'Z' &&
           Data[3] == 'Z';  // :‑<
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FuzzMe(Data, Size);
    return 0;
}

#if 0
int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    std::cout << "FuzzingExperiments\n";

    return EXIT_SUCCESS;
}
#endif
//============================================================================
// Name        : SharedMemory.h
// Created on  : 12.06.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : SharedMemory
//============================================================================

#ifndef CPPPROJECTS_SHAREDMEMORY_H
#define CPPPROJECTS_SHAREDMEMORY_H

#include <vector>
#include <string_view>

namespace SharedMemory {
    void TestAll(const std::vector<std::string_view>& params);
};

#endif //CPPPROJECTS_SHAREDMEMORY_H

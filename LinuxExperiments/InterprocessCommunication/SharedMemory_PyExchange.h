/**============================================================================
Name        : SharedMemory_PyExchange.h
Created on  : 28.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedMemory_PyExchange.h
============================================================================**/

#ifndef CPPPROJECTS_SHAREDMEMORY_PYEXCHANGE_H
#define CPPPROJECTS_SHAREDMEMORY_PYEXCHANGE_H

#include <vector>
#include <string_view>

namespace SharedMemory_PyExchange
{
    void TestAll(const std::vector<std::string_view>& params);
};

#endif //CPPPROJECTS_SHAREDMEMORY_PYEXCHANGE_H

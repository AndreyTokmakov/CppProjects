/**============================================================================
Name        : SharedMemoryDataExchangeEx.h
Created on  : 22.10.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedMemoryDataExchangeEx.h
============================================================================**/

#ifndef CPPPROJECTS_SHAREDMEMORYDATAEXCHANGEEX_H
#define CPPPROJECTS_SHAREDMEMORYDATAEXCHANGEEX_H

#include <vector>
#include <string_view>

namespace SharedMemoryDataExchangeEx
{
    void TestAll(const std::vector<std::string_view>& params);
};

#endif //CPPPROJECTS_SHAREDMEMORYDATAEXCHANGEEX_H

/**============================================================================
Name        : SharedMemoryDataExchange.h
Created on  : 30.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedMemoryDataExchange.h
============================================================================**/

#ifndef CPPPROJECTS_SHAREDMEMORYDATAEXCHANGE_H
#define CPPPROJECTS_SHAREDMEMORYDATAEXCHANGE_H

#include <vector>
#include <string_view>

namespace SharedMemoryDataExchange {
    void TestAll(const std::vector<std::string_view>& params);
};

#endif //CPPPROJECTS_SHAREDMEMORYDATAEXCHANGE_H
/**============================================================================
Name        : SharedMemoryDataExchangeQueue.h
Created on  : 23.10.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedMemoryDataExchangeQueue.h
============================================================================**/

#ifndef SHAREDMEMORYDATAEXCHANGEQUEUE_H
#define SHAREDMEMORYDATAEXCHANGEQUEUE_H

#include <vector>
#include <string_view>

namespace SharedMemoryDataExchangeQueue
{
    void TestAll(const std::vector<std::string_view>& params);
};



#endif //SHAREDMEMORYDATAEXCHANGEQUEUE_H

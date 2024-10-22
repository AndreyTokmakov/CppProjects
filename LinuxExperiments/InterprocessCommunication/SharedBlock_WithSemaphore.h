/**============================================================================
Name        : SharedBlock_WithSemaphore.h
Created on  : 22.10.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SharedBlock_WithSemaphore.h
============================================================================**/

#ifndef CPPPROJECTS_SHAREDBLOCK_WITHSEMAPHORE_H
#define CPPPROJECTS_SHAREDBLOCK_WITHSEMAPHORE_H

#include <vector>
#include <string_view>

namespace SharedBlock_WithSemaphore
{
    void TestAll(const std::vector<std::string_view>& params);
};

#endif //CPPPROJECTS_SHAREDBLOCK_WITHSEMAPHORE_H

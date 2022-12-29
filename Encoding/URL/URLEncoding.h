/**============================================================================
Name        : URLEncoding.h
Created on  : 11.09.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : URLEncoding
============================================================================**/

#ifndef CPPPROJECTS_URLENCODING_H
#define CPPPROJECTS_URLENCODING_H

#include <vector>
#include <string_view>

namespace URLEncoding {
    void TestAll(const std::vector<std::string_view>& params);
};

#endif //CPPPROJECTS_URLENCODING_H

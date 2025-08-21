/**============================================================================
Name        : Base64.h
Created on  : 11.09.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : Base64
============================================================================**/

#ifndef CPPPROJECTS_BASE64_HPP
#define CPPPROJECTS_BASE64_H

#include <vector>
#include <string_view>

namespace Base64 {
    void TestAll(const std::vector<std::string_view>& params);
};

#endif //CPPPROJECTS_BASE64_HPP

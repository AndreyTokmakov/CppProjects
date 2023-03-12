/**============================================================================
Name        : ContikiDCube.h
Created on  : 12.03.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ContikiDCube
============================================================================**/

#ifndef CPPPROJECTS_CONTIKIDCUBE_H
#define CPPPROJECTS_CONTIKIDCUBE_H

#include <string_view>
#include <vector>

namespace ContikiDCube {
    void TestAll([[maybe_unused]] const std::vector<std::string_view>& params);
};

#endif //CPPPROJECTS_CONTIKIDCUBE_H

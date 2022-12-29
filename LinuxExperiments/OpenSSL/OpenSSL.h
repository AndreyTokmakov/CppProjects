//============================================================================
// Name        : OpenSSL.h
// Created on  : 12.07.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : OpenSSL
//============================================================================

#ifndef CPPPROJECTS_OPENSSL_H
#define CPPPROJECTS_OPENSSL_H

#include <vector>
#include <string_view>

namespace OpenSSL {
    void TestAll(const std::vector<std::string_view>& params);
};

#endif //CPPPROJECTS_OPENSSL_H

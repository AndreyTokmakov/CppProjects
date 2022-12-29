//============================================================================
// Name        : HTTPResponse.h
// Created on  : December 07, 2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Http request class
//============================================================================

#ifndef HTTP_RESPONSE_INCLUDE_GUARD
#define HTTP_RESPONSE_INCLUDE_GUARD

#include <iostream>
#include <string>
#include <vector>
#include "Header.h"

namespace Http {

    // HTTPResponse class.
    class HTTPResponse {
    public:
        /** HTTP Headers. **/
        std::vector<Header> headers;

        /** Data **/
        std::string data {};
    };
}

#endif // !HTTP_RESPONSE_INCLUDE_GUARD
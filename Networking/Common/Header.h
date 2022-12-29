//============================================================================
// Name        : Header.h
// Created on  : December 07, 2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Http Headers class
//============================================================================

#ifndef HTTP_HEADER_INCLUDE_GUARD
#define HTTP_HEADER_INCLUDE_GUARD

#include <iostream>
#include <string>
#include "HTTPHeader.h"

namespace Http {

    // Header class.
    struct Header
    {
        Http::HTTPHeader name;
        std::string value;
    };

}

#endif // !HTTP_HEADER_INCLUDE_GUARD
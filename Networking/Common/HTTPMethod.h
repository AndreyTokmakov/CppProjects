//============================================================================
// Name        : HTTPMethod.h
// Created on  : December 11, 2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : HTTP Request method
//============================================================================

#ifndef HTTP_METHOD_INCLUDE_GUARD
#define HTTP_METHOD_INCLUDE_GUARD

namespace Http {

    enum class Method {
        None = 0,
        GET = 1,
        POST = 2,
        HEAD = 3,
        PUT = 4,
        // DELETE = 5,
        CONNECT = 6,
        OPTIONS = 7,
        TRACE = 8,
        PATCH = 9
    };

}

#endif // !HTTP_METHOD_INCLUDE_GUARD
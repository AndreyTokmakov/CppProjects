//============================================================================
// Name        : HTTPRequest.h
// Created on  : December 07, 2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Http request class
//============================================================================

#ifndef HTTP_REQUEST_INCLUDE_GUARD
#define HTTP_REQUEST_INCLUDE_GUARD

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Header.h"
#include "HTTPMethod.h"
#include "HTTPHeader.h"

namespace Http {

    // HTTPRequestEx class.
    class HTTPRequest {
    private:
        /** HTTP request method. **/
        Method method{ Method::None };

        /** HTTP Method string. **/
        std::string method_string{ "" };

        /** HTTP request path/context. **/
        std::string path{ "" };

        /** HTTP params string. **/
        std::string params{ "" };

        /** HTTP Headers. **/
        std::vector<Header> headers;

    protected:
        bool parse_method_string();
        Method HttpMethodFromString();

    public:
        // TODO: rename!
        bool parse_input(const std::string& http_request);

    public:
        HTTPRequest() = default;
        HTTPRequest(const std::string& http_request);
        HTTPRequest(std::string&& http_request);

        std::string toString() const noexcept;

        inline const std::string getPath() const noexcept {
            return this->path;
        }

        inline Method getMethod() const noexcept {
            return this->method;
        }
    };
}

#endif // !HTTP_REQUEST_EX_INCLUDE_GUARD
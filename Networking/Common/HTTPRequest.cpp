//============================================================================
// Name        : HTTPRequestEx.cpp
// Created on  : December 07, 2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Http request class
//============================================================================

#include "HTTPRequest.h"
#include <array>

namespace Http {

    // Some local HTTP specific constants:
    namespace {
        /** HTTP headers delimiter **/
        inline static const std::string DELIMITER = "\r\n";

        /** Separates http request method and path: **/
        inline static const std::string PATH_SEPARATOR = " /";

        /** Separates http request method and path: **/
        inline static const std::string HEADER_SEPARATOR = ": ";

        /** Separates http request path and params parts: **/
        inline static const std::string HEADER_PARAMS_SEPARATOR = "?";

        /** Separates http request path and HTTP version: **/
        inline static const std::string HTTP_VERION_SEPARATOR = R"( HTTP)";

        using HttpMethodDef = std::pair<std::string_view, Method>;
        static constexpr std::array<HttpMethodDef, 8> HTTP_METHODS_MAPPING{ {
                                                                                    {std::string_view("GET"),     Method::GET},
                                                                                    {std::string_view("POST"),    Method::POST},
                                                                                    {std::string_view("HEAD"),    Method::HEAD},
                                                                                    {std::string_view("PUT"),     Method::PUT},
                                                                                    //{std::string_view("DELETE"),  Method::DELETE},
                                                                                    {std::string_view("CONNECT"), Method::CONNECT},
                                                                                    {std::string_view("OPTIONS"), Method::OPTIONS},
                                                                                    {std::string_view("TRACE"),   Method::TRACE},
                                                                                    {std::string_view("PATCH"),   Method::PATCH}
                                                                            } };
    }

    HTTPRequest::HTTPRequest(const std::string& http_request) {
        parse_input(http_request);
    }

    HTTPRequest::HTTPRequest(std::string&& http_request) {
        parse_input(std::move(http_request));
    }

    std::string HTTPRequest::toString() const noexcept
    {
        std::string tmp{ method_string };
        tmp.append("\r\n");
        for (const auto& [k, v] : headers)
            tmp.append(Http::nameByHeader(k))
                    .append(HEADER_SEPARATOR)
                    .append(v)
                    .append(DELIMITER);
        return tmp;
    }

    // TODO: Check performance
    //       We have copy http request string parts to HTTP headers collection
    //       But there is no other options we have ((( we have to keep exact data itself to postprocessing later
    //       But std::string do not offer functionality to extract the part of it (only copy)

    bool HTTPRequest::parse_input(const std::string& http_request)
    {
        // Clear headers.
        headers.clear();

        // Extract the HTTP method string
        size_t pos = http_request.find(DELIMITER, 0);
        if (std::string::npos == pos)
            return false;

        /** Set the method string. **/
        method_string.assign(http_request.begin(), http_request.begin() + pos);
        parse_method_string();

        // Extract all other HTTP headers:
        size_t prev = pos + DELIMITER.length(), head_pos = std::string::npos;
        while ((pos = http_request.find(DELIMITER, prev)) != std::string::npos)
        {
            head_pos = http_request.find(HEADER_SEPARATOR, prev);
            if (std::string::npos != head_pos && pos > head_pos)
            {
                auto& [name, value] = headers.emplace_back();
                name = Http::headerByName(std::string_view(http_request.begin() + prev,
                                                           http_request.begin() + head_pos));
                value.assign(http_request.begin() + head_pos + HEADER_SEPARATOR.length(),
                             http_request.begin() + pos);
            }
            prev = pos + DELIMITER.length();
        }
        // TODO: handle POST request
        return true;
    }

    bool HTTPRequest::parse_method_string()
    {
        size_t pos = method_string.find(PATH_SEPARATOR);
        if (std::string::npos == pos)
            return false;

        // Set the HTTP method:
        method = HttpMethodFromString();

        size_t pos1 = method_string.find(HEADER_PARAMS_SEPARATOR, pos);
        size_t pos2 = method_string.find(HTTP_VERION_SEPARATOR, std::string::npos != pos1 ? pos1 : pos);
        if (std::string::npos == pos2)
            return false;

        if (std::string::npos != pos1) {
            path.assign(method_string.begin() + pos + PATH_SEPARATOR.size(),
                        method_string.begin() + pos1);
            params.assign(method_string.begin() + pos1 + HEADER_PARAMS_SEPARATOR.size(),
                          method_string.begin() + pos2);
        }
        else {
            path.assign(method_string.begin() + pos + PATH_SEPARATOR.size(),
                        method_string.begin() + pos2);
        }

        return true;
    }

    Method HTTPRequest::HttpMethodFromString() {
        for (const auto& def : HTTP_METHODS_MAPPING) {
            for (size_t i = 0; i < method_string.size() && i <= def.first.size(); ++i) {
                if (' ' == method_string[i])
                    return def.second;
                if (def.first[i] != method_string[i])
                    break;
            }
        }
        return Method::None;
    }
}
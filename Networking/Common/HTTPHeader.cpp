//============================================================================
// Name        : HTTPHeader.h
// Created on  : April 23, 2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : HTTP Header method
//============================================================================

#include "HTTPHeader.h"

namespace Http {

    HTTPHeader headerByName(std::string_view header_name) {
        for (const auto& def : HTTP_HEADERS_MAPPING) {
            if (0 == header_name.compare(def.first))
                return def.second;
        }
        return HTTPHeader::None;
    }

    std::string nameByHeader(HTTPHeader header) {
        for (const auto& def : HTTP_HEADERS_MAPPING) {
            if (def.second == header)
                return std::string(def.first);
        }
        return std::string("None");
    }

}
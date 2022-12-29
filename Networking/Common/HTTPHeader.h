//============================================================================
// Name        : HTTPHeader.h
// Created on  : April 23, 2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : HTTP Header method
//============================================================================

#ifndef HTTP_HEADERS_INCLUDE_GUARD
#define HTTP_HEADERS_INCLUDE_GUARD

#include <iostream>
#include <string>
#include <string_view>
#include <array>

namespace Http {

    enum class HTTPHeader : unsigned short {
        None,
        ACCEPT,
        ACCEPT_CHARSET,
        ACCEPT_ENCODING,
        ACCEPT_LANGUAGE,
        ACCEPT_RANGES,
        AGE,
        ALLOW,
        ALTERNATES,
        AUTHORIZATION,
        CACHE_CONTROL,
        CONNECTION,
        CONTENT_DISPOSITION,
        CONTENT_ENCODING,
        CONTENT_LANGUAGE,
        CONTENT_LENGTH,
        CONTENT_LOCATION,
        CONTENT_MD5,
        CONTENT_RANGE,
        CONTENT_TYPE,
        CONTENT_VERSION,
        DATE,
        DAV,
        DEPTH,
        DESTINATION,
        ETAG,
        EXPECT,
        EXPIRES,
        FROM,
        HOST,
        IF,
        IF_MATCH,
        IF_MODIFIED_SINCE,
        IF_NONE_MATCH,
        IF_RANGE,
        IF_UNMODIFIED_SINCE,
        LAST_MODIFIED,
        LINK,
        LOCATION,
        LOCK_TOKEN,
        MAX_FORWARDS,
        MIME_VERSION,
        OVERWRITE,
        PRAGMA,
        PROXY_AUTHENTICATE,
        PROXY_AUTHORIZATION,
        PUBLIC,
        RANGE,
        REFERER,
        RETRY_AFTER,
        SERVER,
        STATUS_URI,
        TE,
        TIMEOUT,
        TITLE,
        TRAILER,
        TRANSFER_ENCODING,
        UPGRADE,
        USER_AGENT,
        VARY,
        VIA,
        WARNING,
        WWW_AUTHENTICATE
    };

    static constexpr size_t HEADERS_SIZE { 54 };

    using HttpHeaderAlias = std::pair<std::string_view, HTTPHeader>;
    static constexpr std::array<HttpHeaderAlias, HEADERS_SIZE> HTTP_HEADERS_MAPPING{ {
        { std::string_view("Accept"),  HTTPHeader::ACCEPT },
        { std::string_view("Accept-Charset"),  HTTPHeader::ACCEPT_CHARSET },
        { std::string_view("Accept-Encoding"),  HTTPHeader::ACCEPT_ENCODING },
        { std::string_view("Accept-Language"),  HTTPHeader::ACCEPT_LANGUAGE },
        { std::string_view("Accept-Ranges"),  HTTPHeader::ACCEPT_RANGES },
        { std::string_view("Age"),  HTTPHeader::AGE },
        { std::string_view("Allow"),  HTTPHeader::ALLOW },
        { std::string_view("Alternates"),  HTTPHeader::ALTERNATES },
        { std::string_view("Authorization"),  HTTPHeader::AUTHORIZATION },
        { std::string_view("Cache-Control"),  HTTPHeader::CACHE_CONTROL },
        { std::string_view("Connection"),  HTTPHeader::CONNECTION },
        { std::string_view("Content-Encoding"),  HTTPHeader::CONTENT_ENCODING },
        { std::string_view("Content-Disposition"),  HTTPHeader::CONTENT_DISPOSITION },
        { std::string_view("Content-Language"),  HTTPHeader::CONTENT_LANGUAGE },
        { std::string_view("Content-Length"),  HTTPHeader::CONTENT_LENGTH },
        { std::string_view("Content-Location"),  HTTPHeader::CONTENT_LOCATION },
        { std::string_view("Content-MD5"),  HTTPHeader::CONTENT_MD5 },
        { std::string_view("Content-Range"),  HTTPHeader::CONTENT_RANGE },
        { std::string_view("Content-Type"),  HTTPHeader::CONTENT_TYPE },
        { std::string_view("Content-Version"),  HTTPHeader::CONTENT_VERSION },
        { std::string_view("Date"),  HTTPHeader::DATE },
        //{ std::string_view("Derived-From"),  HTTPHeader::Derived - From },
        { std::string_view("ETag"),  HTTPHeader::ETAG },
        { std::string_view("Expect"),  HTTPHeader::EXPECT },
        { std::string_view("Expires"),  HTTPHeader::EXPIRES },
        { std::string_view("From"),  HTTPHeader::FROM },
        { std::string_view("Host"),  HTTPHeader::HOST },
        { std::string_view("If-Match"),  HTTPHeader::IF_MATCH },
        { std::string_view("If-Modified-Since"),  HTTPHeader::IF_MODIFIED_SINCE },
        { std::string_view("If-None-Match"),  HTTPHeader::IF_NONE_MATCH },
        { std::string_view("If-Range"),  HTTPHeader::IF_RANGE },
        { std::string_view("If-Unmodified-Since"),  HTTPHeader::IF_UNMODIFIED_SINCE },
        { std::string_view("Last-Modified"),  HTTPHeader::LAST_MODIFIED },
        { std::string_view("Link"),  HTTPHeader::LINK },
        { std::string_view("Location"),  HTTPHeader::LOCATION },
        { std::string_view("Max-Forwards"),  HTTPHeader::MAX_FORWARDS },
        { std::string_view("MIME-Version"),  HTTPHeader::MIME_VERSION },
        { std::string_view("Pragma"),  HTTPHeader::PRAGMA },
        { std::string_view("Proxy-Authenticate"),  HTTPHeader::PROXY_AUTHENTICATE },
        { std::string_view("Proxy-Authorization"),  HTTPHeader::PROXY_AUTHORIZATION },
        { std::string_view("Public"),  HTTPHeader::PUBLIC },
        { std::string_view("Range"),  HTTPHeader::RANGE },
        { std::string_view("Referer"),  HTTPHeader::REFERER },
        { std::string_view("Retry-After"),  HTTPHeader::RETRY_AFTER },
        { std::string_view("Server"),  HTTPHeader::SERVER },
        { std::string_view("Title"),  HTTPHeader::TITLE },
        { std::string_view("TE"),  HTTPHeader::TE },
        { std::string_view("Trailer"),  HTTPHeader::TRAILER },
        { std::string_view("Transfer-Encoding"),  HTTPHeader::TRANSFER_ENCODING },
        { std::string_view("Upgrade"),  HTTPHeader::UPGRADE },
        { std::string_view("User-Agent"),  HTTPHeader::USER_AGENT },
        { std::string_view("Vary"),  HTTPHeader::VARY },
        { std::string_view("Via"),  HTTPHeader::VIA },
        { std::string_view("Warning"),  HTTPHeader::WARNING },
        { std::string_view("WWW-Authenticate"),  HTTPHeader::WWW_AUTHENTICATE }
    } };

    HTTPHeader headerByName(std::string_view header_name);
    std::string nameByHeader(HTTPHeader header);
}

#endif // !HTTP_HEADERS_INCLUDE_GUARD
//============================================================================
// Name        : URL.h
// Created on  : 07.12.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ URL class
//============================================================================

#include "URL.h"

#include <iostream>
#include <string>
#include <string_view>
#include <stdexcept>
#include <regex>

namespace URL::Impl1 {

    class URL {
    private:
        std::string url;

    public:
        // URL(std::string url);

        URL() {}

        // bool isAbsolute();
        // bool isRelative();

        inline void setURL(const std::string &url) noexcept {
            this->url = url;
        }

        [[nodiscard]]
        inline std::string toString() const noexcept {
            return url;
        }

        static std::string getFirstPiece(const std::string &url) {
            std::size_t found = url.find("//");
            if (found == std::string::npos) {
                found = url.find('/');
            } else {
                found = url.find('/', found + 2);
            }

            if (found == std::string::npos)
                return url;
            std::string firstPiece = url.substr(0, found);
            return firstPiece;
        }

        static std::string toBaseURL(const std::string &url) {
            // if the url is http://example.com, ignore the / in the :// part
            std::size_t toIgnore = url.find("://");
            std::size_t found = url.find_last_of('/');
            if (found == std::string::npos || found < toIgnore + 3)
                return url + "/";

            return url.substr(0, found + 1);
        }


        [[nodiscard]]
        bool isValidAbsolute() const noexcept {
            constexpr std::string_view urlRegexStr = "(http|ftp|https)://([\\w_-]+(?:(?:\\.[\\w_-]+)+))"
                                                     "([\\w.,@?^=%&:/~+#-]*[\\w@?^=%&/~+#-])?";
            const std::regex urlRegex(urlRegexStr.data());
            return std::regex_match(url, urlRegex);
        }


        void toAbsolute(const std::string &relativeToUrl) {
            if (!isValidAbsolute()) {
                return;
            }
            //convert to base url: example.com/example/ instead of example.com/example/index.php
            std::string baseURL = URL::toBaseURL(relativeToUrl);
            std::cout << "Bae url: " << baseURL << std::endl;
            std::string absoluteUrl;
            std::size_t found = url.find('/');
            if (found != std::string::npos) {
                //url begins with /
                if (found == 0) {
                    // domain name<relative path> get the domain name
                    absoluteUrl = URL::getFirstPiece(relativeToUrl) + this->url;
                } //there is a / , so subfolder
                else { //find last / of base url and append the url to this piece
                    absoluteUrl = baseURL + this->url;
                }
            } //url is something like second.php
            else { //append the url to base url
                absoluteUrl = baseURL + this->url;
            }
            this->url = absoluteUrl;
        }
    };

    //-----------------------------------------------------------------------------------------

    void isValidTests() {
        URL url;
        url.setURL("http://exampl?e.com/part1/part+2");

        std::cout << std::boolalpha << url.isValidAbsolute() << std::endl;
    }

    void Tests()
    {
        URL url;
        url.setURL("http://example.com/part1/part2/index.php");

        std::cout << url.toString() << std::endl;
        std::cout << URL::getFirstPiece(url.toString()) << std::endl;
        std::cout << URL::toBaseURL(url.toString()) << std::endl;

    }
};


namespace URL::Impl2 {

    class URL {
    private:
        std::string url;

    public:
        // URL(std::string&& _url): url { std::move(_url) } { }
        URL(std::string_view _url): url { _url.data()} { }

        void setUrl(std::string_view _url) {
            url.assign(_url.data());
        }

        [[nodiscard]]
        inline std::string asString() const noexcept {
            return url;
        }

        [[nodiscard]]
        inline std::string_view asStringView() const noexcept {
            return std::string_view(url);
        }

        [[nodiscard]]
        std::string getBaseAddress() const noexcept {
            const std::size_t pos1 = url.find("://");
            if (std::string::npos == pos1)
                return url;

            const std::size_t pos2 = url.rfind('/');
            if (pos1 + 2 == pos2)
                return url;

            return url.substr(0, pos2);
        }

        [[nodiscard]]
        std::string getHost() const noexcept {
            std::size_t pos1 = url.find("://");
            if (std::string::npos == pos1)
                return {};

            pos1 += 3;
            std::size_t pos2 = url.find('/', pos1);
            if (std::string::npos == pos2)
                pos2 = url.length();

            return url.substr(pos1, pos2 - pos1);
        }
    };

    //---------------------------------------------------------------------

    void Tests() {
        URL url("http://test.example.com/part1/part2/part3/index.hyml");

        std::cout << url.asString() << std::endl;
        std::cout << url.asStringView() << std::endl;
        std::cout << url.getBaseAddress() << std::endl;
        std::cout << url.getHost() << std::endl;
    }
};

void URL::TestAll()
{
    // Impl1::Tests();
    Impl2::Tests();

};

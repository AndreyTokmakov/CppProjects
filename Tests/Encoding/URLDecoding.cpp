//============================================================================
// Name        : URLDecoding.h
// Created on  : 30.01.2022
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ URLDecoding src
//============================================================================

#include "URLDecoding.h"

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <array>


namespace URLDecoding
{
    char from_hex(char ch) {
        return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
    }

    std::string url_decode(std::string text) {
        char h;
        std::ostringstream escaped;
        escaped.fill('0');

        for (auto i = text.begin(), n = text.end(); i != n; ++i) {
            std::string::value_type c = (*i);

            if (c == '%') {
                if (i[1] && i[2]) {
                    h = from_hex(i[1]) << 4 | from_hex(i[2]);
                    escaped << h;
                    i += 2;
                }
            } else if (c == '+') {
                escaped << ' ';
            } else {
                escaped << c;
            }
        }

        return escaped.str();
    }

    std::string urlEncode(std::string str){
        std::string new_str = "";
        int ic;
        const char* chars = str.c_str();
        char bufHex[10], c;

        for(int i = 0, size = str.size(); i < size; ++i){
            c = chars[i];
            ic = c;
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
                new_str += c;
            else {
                sprintf(bufHex,"%X",c);
                if(ic < 16)
                    new_str += "%0";
                else
                    new_str += "%";
                new_str += bufHex;
            }
        }
        return new_str;
    }

    void Test()
    {
        const std::string msg = "ya.ru";
        std::cout << msg << "   ---->   " << url_decode(msg) << std::endl;
        std::cout << msg << "   ---->   " << urlEncode(msg) << std::endl;

    }

};

void URLDecoding::TestAll()
{
    Test();
};


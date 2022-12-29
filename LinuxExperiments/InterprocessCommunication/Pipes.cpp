//============================================================================
// Name        : Pipes.cpp
// Created on  : 14.06.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Pipes
//============================================================================

#include "Pipes.h"
#include "../common.h"

#include <iostream>
#include <array>

namespace Pipes {

    void ReportError(std::string_view errText) {
        std::cout << errText << ". Error = " << errno << std::endl;
    }

    void SimpleTest()
    {
        std::array<int, 2> pipeFd {};
        auto& [readFd, writeFd] = pipeFd;

        constexpr size_t dataSize {6};
        std::string buffer(dataSize, '\0');

        if (-1 == pipe(pipeFd.data()))
            return ReportError("Pipe()");

        ::write(writeFd, "qwerty", dataSize);   /** writing a string "code" in pipe **/
        ::read(readFd, buffer.data(),dataSize); /** reading pipe now buff is equal to "code" **/

        buffer.resize(dataSize);
        std::cout << buffer << std::endl;

        /** Close pipe handlers: **/
        std::for_each(pipeFd.begin(), pipeFd.end(), [] (int fd) { ::close(fd); });
    }

}

void Pipes::TestAll(const std::vector<std::string_view>& params)
{
    SimpleTest();
};


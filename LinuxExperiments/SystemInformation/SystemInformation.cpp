//============================================================================
// Name        : SystemInformation.h
// Created on  : 10.06.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : SystemInformation
//============================================================================

#include "SystemInformation.h"
#include <iostream>

#include "../common.h"

namespace SystemInformation
{
    void GetSysInfo() {
        utsname buf1 {};
        if (uname(&buf1)!=0) {
            std::cout << "uname() failed. Error = " << errno << std::endl;
            return;
        }

        std::cout << "System Name = " << buf1.sysname << std::endl;
        std::cout << "Node Name = "   << buf1.nodename << std::endl;
        std::cout << "Version = "     << buf1.version << std::endl;
        std::cout << "Release = "     << buf1.release << std::endl;
        std::cout <<"Machine = "      << buf1.machine << std::endl;
    }

    void GetPageSize()
    {
        // https://www.opennet.ru/man.shtml?topic=mmap&category=2&russian=0
        std::cout << "Page size: " << sysconf(_SC_PAGESIZE) << " bytes" << std::endl;

        std::cout << "Page size: " << getpagesize() << " bytes" << std::endl;
    }
};

void SystemInformation::TestAll()
{
    // GetSysInfo();
    // GetPageSize();

    std::cout << sysconf(_SC_LOGIN_NAME_MAX)  << std::endl;
    std::cout << sysconf(_SC_OPEN_MAX)  << std::endl;
    std::cout << sysconf(_SC_NGROUPS_MAX)  << std::endl;
    std::cout << sysconf(_SC_RTSIG_MAX)  << std::endl;
};
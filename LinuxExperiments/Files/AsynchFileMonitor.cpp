/**============================================================================
Name        : AsynchFileMonitor.cpp
Created on  : 15.10.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AsynchFileMonitor.cpp
============================================================================**/

#include "AsynchFileMonitor.hpp"

#include "FileUtilities.hpp"
#include "FinalAction.hpp"

#include <iostream>
#include <filesystem>
#include <print>
#include <format>
#include <memory>
#include <vector>


#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

namespace
{
    constexpr std::filesystem::path testDataDir() noexcept
    {
        return std::filesystem::current_path() / "../../resources";
    }
}

namespace AsynchFileMonitor::Demo_1
{




    bool setNonBlocking(const int fd)
    {
        const int flags = ::fcntl(fd, F_GETFL, 0);
        if (flags < 0) {
            std::cerr << "fcntl() failed. F_GETFL " << std::endl;
            return false;
        }

        if (-1 == ::fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
            std::cerr << "fcntl() failed. (F_SETFL && O_NONBLOCK)" << std::endl;
            return false;
        }

        return true;
    }

    void run()
    {
        const std::filesystem::path testFile = testDataDir() / "test_file.txt";
        const char* path = testFile.c_str();

        const int fileHandle = ::open(path, O_RDONLY | O_NONBLOCK);
        if (-1 == fileHandle) {
            std::cerr << "Failed to open file. Error = " << errno << std::endl;
            return;
        }

        auto cleanup = [&fileHandle] {
            std::cout << "Cleanup (fileHandle: " << fileHandle << ")\n";
            ::close(fileHandle);
        };
        utilities::final_action::ScopeExit onExit(cleanup);

        if (!setNonBlocking(fileHandle)) {
            return;
        }

        const int epollFd = ::epoll_create1(0);
        if (-1 == epollFd) {
            std::cerr << "Failed to create epoll file descriptor" << std::endl;
            return;
        }

        epoll_event event {
            .events = EPOLLIN,
            .data = epoll_data { .fd = 0 }
        };

        const int32_t result = ::epoll_ctl(epollFd, EPOLL_CTL_ADD, fileHandle, &event);
        if (0 != result) {
            std::cerr << "Failed to add file descriptor to epoll. result = " << result  <<
                ", Error = " << errno << std::endl;
            return;
        }

        constexpr uint32_t EVENTS_MAX = 64;
        std::array<epoll_event, EVENTS_MAX> epollEvents {};
        std::array<char, 128> buffer {};
        while (true)
        {
            // Call epoll_wait with a timeout of 1000 milliseconds.
            const int32_t readyCount = epoll_wait(epollFd, epollEvents.data(), EVENTS_MAX, 1000);
            std::cout << "readyCount: " << readyCount << std::endl;

            // Function epoll_wait returned.
            if (-1 == readyCount) {
                std::cerr << "epoll_wait() failed. Error = " << errno << std::endl;
                break;
            }

            // Handle any file descriptors with events.
            for (int i = 0; i < readyCount; i++)
            {
                const epoll_event& epoll_event = epollEvents[i];

                // Handle the event now, by reading in data and printing it.
                const int fd = epoll_event.data.fd;
                const ino64_t bytesRead = ::read(fd, buffer.data(), buffer.size() -1);

                std::cout << "Bytes read: " << bytesRead << std::endl;
                std::cout << "Data" << std::string_view(buffer.data(), bytesRead)  << std::endl;
            }
        }

        ::close(fileHandle);
    }
}


void AsynchFileMonitor::TestAll()
{
    Demo_1::run();
}
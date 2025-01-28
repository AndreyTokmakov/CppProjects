/**============================================================================
Name        : AsynchFileReader.cpp
Created on  : 28.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AsynchFileReader.cpp
============================================================================**/

#include "AsynchFileReader.h"

#include <cstddef>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <memory>
#include <iostream>
#include <array>

namespace AsynchFileReader
{
    struct FileReader
    {
        static constexpr size_t EVENTS_MAX = 64;
        static constexpr size_t READ_BUFFER_SIZE = 512;

        static_assert(EVENTS_MAX > 0, "EVENTS_MAX must be greater than 0");
        static_assert(READ_BUFFER_SIZE > 0, "READ_BUFFER_SIZE must be greater than 0");

        FileReader() : epoll_fd_ { ::epoll_create1(0) }, stopped { false } {
            std::cout << "epoll_fd_ = " << epoll_fd_ << std::endl;
        }

        ~FileReader()
        {
            ::close(epoll_fd_);
        }

        void addFileDescriptor(const int fd)
        {
            epoll_event epoll_event {};
            epoll_event.events = EPOLLIN;
            epoll_event.data.fd = fd;

            std::cout << "addFileDescriptor. fd = " << fd << std::endl;

            const int result = ::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &epoll_event);
            std::cout << "addFileDescriptor. result = " << result << std::endl;
            if (-1 == result)
            {
                std::cerr << errno << std::endl;
            }
        }

        void removeFileDescriptor(const int fd)
        {
            epoll_event epoll_event {};
            ::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &epoll_event);
        }

        void runAsyncLoop()
        {
            std::array<epoll_event, EVENTS_MAX> epollEvents {};
            while(!stopped)
            {
                // Call epoll_wait with a timeout of 1000 milliseconds.
                const int fds_ready = ::epoll_wait(epoll_fd_, epollEvents.data(), EVENTS_MAX, 1000);

                std::cout << __LINE__ << std::endl;
                // Function epoll_wait returned.
                if(-1 == fds_ready) {
                    // An error occurred.
                    stopped = true;
                    continue;
                }

                std::cout << __LINE__ << "| fds_ready = " << fds_ready << std::endl;

                // Handle any file descriptors with events.
                for (int i = 0; i < fds_ready; i++) {
                    std::cout << __LINE__ << std::endl;
                    const epoll_event& epoll_event = epollEvents[i];
                    handleEventOnFile(epoll_event);
                }
            }
        }

        void stopAsyncLoop()
        {
            stopped = true;
        }

        [[nodiscard]]
        bool hasStopped() const noexcept
        {
            return stopped;
        }

    private:

        int epoll_fd_;
        bool stopped {true };
        char read_buffer_[READ_BUFFER_SIZE] {};

        void handleEventOnFile(const epoll_event& epoll_event)
        {
            const uint32_t events = epoll_event.events;
            const int fd = epoll_event.data.fd;

            std::cout << __LINE__ << std::endl;
            if (EPOLLERR & events)
            {
                std::cout << __LINE__ << std::endl;
                // An error occured on the file descriptor. Try to close it.
                removeFileDescriptor(fd);
                ::close(fd);
            }
            else if (EPOLLIN & events)
            {
                // Read is available, read in from the file descriptor and print the message.
                int64_t read_result = ::read(fd, read_buffer_, READ_BUFFER_SIZE-1);
                std::cout << __LINE__ << std::endl;
                if (-1 == read_result)
                {
                    // An error occured while reading.
                    removeFileDescriptor(fd);
                    ::close(fd);
                    return;
                }

                // Successfully read in some bytes.
                read_buffer_[read_result] = '\0';
                printf("Read %d bytes\n", read_result);
                printf("%s", read_buffer_);
            }
        }
    };
}

void AsynchFileReader::TestAll()
{

    // INFO: epoll_ctl() not working with ordinary file errno = EPERM

    FileReader afr;

    /*
    auto deleter = [](auto x) { delete x; };
    std::unique_ptr<std::FILE, decltype(deleter)> ptr {
        std::fopen(R"(/tmp/dir_for_testing/test_file.txt)", "r"), deleter
    };*/

    // int fd = ::open("/tmp/dir_for_testing/test_file.txt", O_RDONLY );
    int fd = ::open("/home/andtokm/DiskS/Temp/TESTING_ROOT_DIR/test_run.log", O_RDONLY );

    afr.addFileDescriptor(fd);
    afr.runAsyncLoop();

}
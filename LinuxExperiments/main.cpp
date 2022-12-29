//============================================================================
// Name        : Tests.cpp
// Created on  : 15.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Tests C++ project
//============================================================================

#include <iostream>
#include <optional>
#include <memory>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <tuple>
#include <experimental/propagate_const>

#include <thread>
#include <future>

#include <mutex>
#include <atomic>

#include <cmath>
#include <numeric>
#include <numbers>
#include <ostream>

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <array>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <span>
#include <concepts>

#include "common.h"

#include <poll.h>
#include <sys/inotify.h>

#include "Files/Files.h"
#include "Processes/Processes.h"
#include "Logs/Logs.h"
#include "ShellCommands/ShellCommands.h"
#include "Environment/Environment.h"

#include "TestsAndExperiments/TestAndExperiments.h"
#include "TestsAndExperiments/FindProcessForTCPConnection.h"

#include "SystemInformation/SystemInformation.h"

#include "Networking/Servers.h"
#include "Networking/WebServerSimple.h"
#include "Networking/Utilities.h"


#include "InterprocessCommunication/Semaphore.h"
#include "InterprocessCommunication/SharedMemory.h"
#include "InterprocessCommunication/Pipes.h"
#include "Users/Users.h"
#include "TimeAndDate/Time.h"
#include "TimeAndDate/Timer.h"
#include "Modules/Modules.h"
#include "OpenSSL/OpenSSL.h"



void PollTest() {
    constexpr size_t eventsCount {0};
    pollfd pfds[eventsCount] {};

    constexpr std::string_view path {R"(/home/andtokm/tmp/TEST_FILES/TestFile.txt)"};
    int fd = open(path.data(), O_RDONLY | O_NONBLOCK);
    if (-1 == fd) {
        std::cout << "Failed to open file '" << path << "'. Error = " << errno << std::endl;
        // TODO: Exit! Close socket!
    } else {
        std::cout << "File '" << path << "' opened\n";
    }

    pfds[0].fd = fd;
    /* Only events in this mask will be listened to.
     * However, there are also some events that are unmaskable notably POLLHUP when pipe closes! */
    pfds[0].events = POLLIN;


    while (true) {
        std::cout << "loop\n";
        int result = poll(pfds, eventsCount, -1);
        if (result == -1) {
            std::cout << "poll() failed. Error = " << errno << std::endl;
            exit(EXIT_FAILURE);
        }

        std::cout << "result = " << result << std::endl;
    }

    if (-1 == close(fd)) {
        std::cout << "Failed to close file" << std::endl;
    } else {
        std::cout << "File is closed" << std::endl;
    }
}

namespace {
    /* Maximum number of events to process at a time: */
    constexpr size_t maxEvents {1024};

    /* We assume that the length of the file name does not exceed 16 characters */
    constexpr size_t nameLength {16};

    /* Size of the Event structure: */
    constexpr size_t eventSize {sizeof(inotify_event)};

    /* Size of the Event structure */
    constexpr size_t bufferSize {maxEvents * (nameLength + eventSize)};
}

void NotifyTest() {
    int monitor = inotify_init1(0);
    if (-1 == monitor) {
        std::cout << "Failed to init Monitor. Error = " << errno << std::endl;
        return;
    }

    constexpr std::string_view path { R"(/home/andtokm/tmp/folder_for_testing)" };
    int wd = inotify_add_watch(monitor, path.data(), IN_CLOSE | IN_MODIFY);
    if (-1 == wd) {
        std::cout << "Failed to create watcher. Error = " << errno << std::endl;
        close(monitor);
        return;
    }

    std::array<char, bufferSize> buffer {};
    size_t i = 0, length = 0;
    while (true) {
        i = 0;
        length = read(monitor, buffer.data(), bufferSize);
        if (0 > length) {
            std::cout << "read() failed. Error = " << errno << std::endl;
            break;
        }

        while (length > i) {
            const auto *event = reinterpret_cast<inotify_event*>(&buffer[i]);
            if (event->len) {
                if ( event->mask & IN_CLOSE) {
                    if (event->mask & IN_ISDIR)
                        std::cout << "The directory '" << event->name << "' was closed.\n";
                    else
                        std::cout << "The file '" << event->name << "' was closed with ID: " << event->wd << std::endl;
                }
                if ( event->mask & IN_MODIFY) {
                    if (event->mask & IN_ISDIR)
                        std::cout << "The directory '" << event->name << "' was modified.\n";
                    else
                        std::cout << "The file '" << event->name << "' was modified with ID: " << event->wd << std::endl;
                }
                i += bufferSize + event->len;
            }
        }
    }
    inotify_rm_watch(monitor, wd);
    close(monitor);
}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> params(argv + 1, argv + argc);

    // Files::TestAll();
    // Processes::TestAll();
    // Logs::TestAll();
    // ShellCommands::TestAll();
    // Environment::TestAll();

    // OpenSSL::TestAll(params);

    Users::TestAll();

    // Time::TestAll();
    // Timer::TestAll();

    // SystemInformation::TestAll();

    // Modules::TestAll();

    // Servers::TestAll();
    // WebServerSimple::TestAll();
    // Networking::Utilities::TestAll();

    // TestAndExperiments::TestAll();
    // FindProcessForTCPConnection::TestAll();

    // NotifyTest();

    // Semaphore::TestAll();
    // SharedMemory::TestAll(params);
    // Pipes::TestAll(params);


    return EXIT_SUCCESS;
}



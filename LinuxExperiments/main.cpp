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


#include "Files/Files.h"
#include "Files/AsynchFileReader.h"

#include "Logs/Logs.h"
#include "ShellCommands/ShellCommands.h"
#include "Environment/Environment.h"

#include "Processes/Processes.h"
#include "Processes/ProcessManager.h"


#include "TestsAndExperiments/TestAndExperiments.h"
#include "TestsAndExperiments/FindProcessForTCPConnection.h"

#include "SystemInformation/SystemInformation.h"

#include "Networking/Servers.h"
#include "Networking/WebServerSimple.h"
#include "Networking/Utilities.h"

#include "InterprocessCommunication/Semaphore.h"
#include "InterprocessCommunication/SharedMemoryWrapper.h"
#include "InterprocessCommunication/SharedMemory.h"
#include "InterprocessCommunication/SharedBlock_WithSemaphore.h"
#include "InterprocessCommunication/SharedMemory_AtomicValue.h"
#include "InterprocessCommunication/SharedMemoryDataExchange.h"
#include "InterprocessCommunication/SharedMemoryDataExchangeEx.h"
#include "InterprocessCommunication/SharedMemoryDataExchangeQueue.h"
#include "InterprocessCommunication/SharedMemory_PyExchange.h"
#include "InterprocessCommunication/Pipes.h"
#include "InterprocessCommunication/MultiprocessQueue.h"
#include "InterprocessCommunication/SemMutex.h"
#include "InterprocessCommunication/SharedMutex.h"
#include "InterprocessCommunication/UnixSockets.h"

#include "IPC_Performance_Experiments/IPC_Performance_Experiments.h"

#include "Users/Users.h"
#include "TimeAndDate/Time.h"
#include "TimeAndDate/Timer.h"
#include "ModulesKernel/ModulesKernel.h"
#include "Modules/Modules.h"
#include "OpenSSL/OpenSSL.h"

#include "IO_Uring/IOUringExperiments.h"
#include "WatchDirectoryChanges/WatchDirectoryChanges.h"


void PollTest()
{
    // FIXME: Size 10 ???
    constexpr size_t eventsCount {10};
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


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> params(argv + 1, argv + argc);

    // Files::TestAll();
    // AsynchFileReader::TestAll();

    // Logs::TestAll();
    // ShellCommands::TestAll();
    // Environment::TestAll();

    // Processes::TestAll();
    // ProcessManager::TestAll();

    // OpenSSL::TestAll(params);

    // Users::TestAll();

    // Time::TestAll();
    // Timer::TestAll();

    // SystemInformation::TestAll();

    // Modules::TestAll();
    // ModulesKernel::TestAll();

    // Servers::TestAll();
    // WebServerSimple::TestAll();
    // Networking::Utilities::TestAll();

    // TestAndExperiments::TestAll();
    // FindProcessForTCPConnection::TestAll();

    // Semaphore::TestAll(params);

    // SharedMemory::TestAll(params);
    // SharedMemoryWrapper::TestAll();
    // SharedMemoryDataExchange::TestAll(params);
    // SharedMemoryDataExchangeEx::TestAll(params);
    // SharedMemoryDataExchangeQueue::TestAll(params);
    // SharedMemory_PyExchange::TestAll(params);
    // SharedBlock_WithSemaphore::TestAll(params);
    // Pipes::TestAll(params);
    // SharedMemory_AtomicValue::TestAll();
    // SharedMutex::TestAll();
    // SemMutex::TestAll();
    // UnixSockets::TestAll();

    /** IPC_Performance_Experiments **/

    UnixDomainSockets::TestAll();
    // UDPSockets::TestAll();

    // IOUringExperiments::TestAll();

    // WatchDirectoryChanges::TestAll();

    return EXIT_SUCCESS;
}



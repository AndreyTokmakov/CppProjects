cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(LinuxExperiments)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

find_package(OpenSSL REQUIRED)
add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

include_directories(${THIRD_PARTY_DIR}/liburing/install_dir/include)
link_directories(${THIRD_PARTY_DIR}/liburing/install_dir/lib)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        Files/Files.cpp
        Files/AsynchFileReader.cpp
        Processes/Processes.cpp
        Logs/Logs.cpp
        ShellCommands/ShellCommands.cpp
        Networking/Servers.cpp
        Networking/WebServerSimple.cpp
        Environment/Environment.cpp
        SystemInformation/SystemInformation.cpp
        InterprocessCommunication/Semaphore.cpp
        InterprocessCommunication/SharedMemory.cpp
        InterprocessCommunication/SharedBlock_WithSemaphore.cpp
        InterprocessCommunication/SharedMemory_AtomicValue.cpp
        InterprocessCommunication/Pipes.cpp
        InterprocessCommunication/SharedMemoryDataExchange.cpp
        InterprocessCommunication/SharedMemoryDataExchangeEx.cpp
        InterprocessCommunication/SharedMemoryDataExchangeQueue.cpp
        InterprocessCommunication/SharedMemoryWrapper.cpp
        InterprocessCommunication/SharedMemory_PyExchange.cpp
        InterprocessCommunication/MultiprocessQueue.cpp
        InterprocessCommunication/SemMutex.cpp
        InterprocessCommunication/SharedMutex.cpp
        InterprocessCommunication/UnixSockets.cpp
        IPC_Performance_Experiments/UnixDomainSockets.cpp
        Users/Users.cpp
        TimeAndDate/Time.cpp
        TimeAndDate/Timer.cpp
        Modules/Modules.cpp
        OpenSSL/OpenSSL.cpp
        Networking/Utilities.cpp
        TestsAndExperiments/TestAndExperiments.cpp
        TestsAndExperiments/FindProcessForTCPConnection.cpp
        Processes/ProcessManager.cpp
        ModulesKernel/ModulesKernel.cpp
        IO_Uring/IOUringExperiments.cpp
        InterprocessCommunication/SharedMemoryDataExchangeQueue.cpp
        WatchDirectoryChanges/WatchDirectoryChanges.cpp
)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME} PUBLIC
        pthread
        rt
        crypt
        crypto
        ssl
        uring
)
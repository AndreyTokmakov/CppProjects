cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(LinuxExperiments)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

find_package(OpenSSL REQUIRED)
add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        Files/Files.cpp Files/Files.h
        Processes/Processes.cpp Processes/Processes.h
        common.h
        Logs/Logs.cpp Logs/Logs.h
        ShellCommands/ShellCommands.cpp ShellCommands/ShellCommands.h
        Networking/Servers.cpp Networking/Servers.h
        Networking/WebServerSimple.cpp Networking/WebServerSimple.h
        Environment/Environment.cpp Environment/Environment.h
        SystemInformation/SystemInformation.cpp SystemInformation/SystemInformation.h
        InterprocessCommunication/Semaphore.cpp
        InterprocessCommunication/Semaphore.h InterprocessCommunication/SharedMemory.cpp
        InterprocessCommunication/SharedMemory.h
        InterprocessCommunication/Pipes.cpp InterprocessCommunication/Pipes.h
        InterprocessCommunication/SharedMemoryDataExchange.cpp InterprocessCommunication/SharedMemoryDataExchange.h
        Users/Users.cpp Users/Users.h
        TimeAndDate/Time.cpp TimeAndDate/Time.h
        TimeAndDate/Timer.cpp TimeAndDate/Timer.h
        Modules/Modules.cpp Modules/Modules.h
        OpenSSL/OpenSSL.cpp OpenSSL/OpenSSL.h Networking/Utilities.cpp Networking/Utilities.h TestsAndExperiments/TestAndExperiments.cpp TestsAndExperiments/TestAndExperiments.h TestsAndExperiments/FindProcessForTCPConnection.cpp TestsAndExperiments/FindProcessForTCPConnection.h Processes/ProcessManager.cpp Processes/ProcessManager.h ModulesKernel/ModulesKernel.cpp ModulesKernel/ModulesKernel.h)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME} PUBLIC
        pthread
        rt
        crypt
        crypto
        ssl
)
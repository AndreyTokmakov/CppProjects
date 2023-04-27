cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(PocoProjects)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_PREFIX_PATH /home/andtokm/DiskS/ProjectsUbuntu/third_party/poco/cmake-build)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

find_package(Poco REQUIRED Foundation Net)

# include all components
add_executable(PocoProjects main.cpp
        Foundation/Events.cpp Foundation/Events.h
        Processes/Processes.cpp Processes/Processes.h
        Networking/Networking.cpp Networking/Networking.h
        Environment/Environment.cpp Environment/Environment.h
        DataTime/Timer.h DataTime/Timer.cpp
        Random/Random.h Random/Random.cpp
        Cryptography/Cryptography.cpp Cryptography/Cryptography.h)

TARGET_LINK_LIBRARIES(PocoProjects
        Poco::Foundation Poco::Net
        pthread
)
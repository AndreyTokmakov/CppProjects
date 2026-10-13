cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(ZeroMQ)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)


add_compile_options(-c -Wall -Wextra -O3 -std=c++26)

include_directories(${THIRD_PARTY_DIR}/cppzmq)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED)

pkg_check_modules(PC_ZMQ libzmq REQUIRED)
find_path(ZMQ_INCLUDE_DIR zmq.h PATHS ${PC_ZMQ_INCLUDE_DIRS})
find_library(ZMQ_LIBRARY NAMES zmq PATHS ${PC_ZMQ_LIBRARY_DIRS})

find_path(CPPZMQ_INCLUDE_DIR zmq.hpp)

set(AZMQ_ROOT ${THIRD_PARTY_DIR}/azmq-master)
find_path(AZMQ_INCLUDE_DIR
    NAMES azmq/socket.hpp
    HINTS "${AZMQ_ROOT}/include" "${AZMQ_ROOT}"
)

if(NOT AZMQ_INCLUDE_DIR)
    message(FATAL_ERROR "Cannot find azmq/socket.hpp. Set AZMQ_ROOT to the local azmq source/install directory.")
endif()

add_executable(${PROJECT_NAME}
        main.cpp
        Common/Logger.cpp
        Experiments/Experiments.cpp
        PubSub/Publisher_Subscriber.cpp
        Request_Response/Request_Response.cpp
        Secure_Communication/Secure_Communication.cpp
        Router_Dealer/Router_Dealer.cpp
        Router_Dealer/Router_Dealer_Experiments.cpp
        Router_Dealer/Router_Dealer_Simple.cpp
        Router_Dealer/Router_Dealer_One.cpp
        Router_Dealer/Router_Dealer_Poller_Timeout.cpp
        Router_Dealer/Router_Dealer_Async_1.cpp
        Router_Dealer/Router_Dealer_Async_2.cpp
        Router_Dealer/Router_Dealer_Async_ServerClass.cpp

        Servers/DebugServer.cpp
        Servers/Servers.cpp
        Servers/ReqRepServer.cpp
        Servers/PollingReqRspServer.cpp

        Proxy/ZeroMqProxy.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/Common"
    "${CMAKE_SOURCE_DIR}/Utilities"
    "${AZMQ_INCLUDE_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    pthread
    utils
    Boost::boost
    Threads::Threads
    ${ZMQ_LIBRARY}
    ${EXTRA_LIBS}
    ${ZMQ_LIBRARY}
)

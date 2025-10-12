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

pkg_check_modules(PC_ZMQ libzmq REQUIRED)
find_path(ZMQ_INCLUDE_DIR zmq.h PATHS ${PC_ZMQ_INCLUDE_DIRS})
find_library(ZMQ_LIBRARY NAMES zmq PATHS ${PC_ZMQ_LIBRARY_DIRS})

find_path(CPPZMQ_INCLUDE_DIR zmq.hpp)

#[[
message("CMAKE_SOURCE_DIR       : ${CMAKE_SOURCE_DIR}")
message("PROJECT_SOURCE_DIR     : ${PROJECT_SOURCE_DIR}")
message("CMAKE_BINARY_DIR       : ${CMAKE_BINARY_DIR}")
message("CMAKE_CURRENT_LIST_DIR : ${CMAKE_CURRENT_LIST_DIR}")
#]]

add_executable(${PROJECT_NAME}
        main.cpp
        Common/Logger.cpp
        Experiments/Experiments.cpp
        PubSub/Publisher_Subscriber.cpp
        Request_Response/Request_Response.cpp
        Secure_Communication/Secure_Communication.cpp
        Router_Dealer/Router_Dealer.cpp
        Router_Dealer_Simple/Router_Dealer_Simple.cpp
        Router_Dealer_One/Router_Dealer_One.cpp
        Router_Dealer_Poller_Timeout/Router_Dealer_Poller_Timeout.cpp
        Router_Dealer_Asynch/RouterDealerAsynch.cpp
        Servers/DebugServer.cpp
        Servers/Servers.cpp
        Servers/ReqRepServer.cpp
        Proxy/ZeroMqProxy.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/Common")

# **** Utilities ****
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/Utilities")
target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/Utilities")


#[[
target_include_directories(${PROJECT_NAME}  PRIVATE
        ${ZMQ_INCLUDE_DIR}
        ${CPPZMQ_INCLUDE_DIR}
)
#]]

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
        utils
        Threads::Threads
        ${ZMQ_LIBRARY}
        ${EXTRA_LIBS}
)
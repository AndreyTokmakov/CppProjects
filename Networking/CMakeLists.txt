cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Networking)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)


add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(Networking MACOSX_BUNDLE
        main.cpp
        Common/Header.cpp Common/Header.h
        Common/HTTPHeader.cpp Common/HTTPHeader.h
        Common/HTTPMethod.h Common/HTTPRequest.cpp
        Common/HTTPRequest.h Common/HTTPResponse.cpp
        Common/HTTPResponse.h
        Common/URL.cpp Common/URL.h
        Common/Utilities.cpp Common/Utilities.h
        Clients/Clients.cpp Clients/Clients.h
        DNS/DNS_ResponseParser.cpp DNS/DNS_ResponseParser.h
        RawSockets/RawSocketsTests.cpp
        RawSockets/RawSocketsTests.h
        Headers/EthernetHeader.h
        Headers/IPv4Header.h
        Headers/TCPHeader.h
        Headers/UDPHeader.h
        Headers/ICMPHeader.h
        Experiments/Experiments.cpp Experiments/Experiments.h
        NetLink/NetLink.cpp NetLink/NetLink.h
        Utilities/Utilities.cpp Utilities/Utilities.h RawSockets/BuildAndSendPacketsTests.cpp RawSockets/BuildAndSendPacketsTests.h Servers/TestWebServer.cpp Servers/TestWebServer.h Headers/ICMPHeader.h Experiments/UtilitiesTests.cpp Experiments/UtilitiesTests.h Clients/UDPClient.cpp Clients/UDPClient.h RawSockets/ARPTester.cpp RawSockets/ARPTester.h Headers/ARPHeader.h Servers/SimpleServerSelect.cpp Servers/SimpleServerSelect.h Servers/SocketTCPServer.cpp Servers/SocketTCPServer.h)

 # Link phread library
TARGET_LINK_LIBRARIES(Networking pthread)
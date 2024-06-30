cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Networking)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

SET(OPENSSL_ROOT_DIR ${THIRD_PARTY_DIR}/openssl)

link_directories(${OPENSSL_ROOT_DIR})
include_directories("${OPENSSL_ROOT_DIR}/include")

include_directories(${THIRD_PARTY_DIR}/libssh2/include)
include_directories(${THIRD_PARTY_DIR}/libssh2/build/src)
link_directories(${THIRD_PARTY_DIR}/libssh2/build/src)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        Common/Header.cpp Common/Header.h
        APIServer/APIServer.cpp APIServer/APIServer.h
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
        Utilities/Utilities.cpp Utilities/Utilities.h
        RawSockets/BuildAndSendPacketsTests.cpp RawSockets/BuildAndSendPacketsTests.h
        Servers/TestWebServer.cpp Servers/TestWebServer.h
        Headers/ICMPHeader.h
        Experiments/UtilitiesTests.cpp
        Experiments/UtilitiesTests.h
        Clients/UDPClient.cpp
        Clients/UDPClient.h
        RawSockets/ARPTester.cpp
        RawSockets/ARPTester.h Headers/ARPHeader.h Servers/SimpleServerSelect.cpp
        Servers/SimpleServerSelect.h Servers/SocketTCPServer.cpp Servers/SocketTCPServer.h
        Servers/EPollTCPServer.cpp Servers/EPollTCPServer.h Servers/EPollTCPServerTwo.cpp
        Servers/EPollTCPServerTwo.h Servers/EPollTCPServerEx.cpp Servers/EPollTCPServerEx.h
        Servers/EPollTCPServerDebug.cpp Servers/EPollTCPServerDebug.h
        Servers/EPollTCPServerMultithreaded.cpp Servers/EPollTCPServerMultithreaded.h
        Servers/EPollTCPServerContext.cpp Servers/EPollTCPServerContext.h
        Servers/EPollTCPServerContextEx.cpp Servers/EPollTCPServerContextEx.h
        RawSockets/CaptureTraffic.cpp RawSockets/CaptureTraffic.h
        PcapAnalyzer/PcapAnalyzer.cpp PcapAnalyzer/PcapAnalyzer.h
        WiFi/WiFiExperiments.cpp WiFi/WiFiExperiments.h
        ssh/SshExperiments.cpp ssh/SshExperiments.h
        SSL/SSL_Experiments.cpp SSL/SSL_Experiments.h
        SSL/TCP_SSL_Server.cpp SSL/TCP_SSL_Server.h
        SSL/HTTPS_Server.cpp SSL/HTTPS_Server.h
        SSL/HTTPS_AsyncServer.cpp SSL/HTTPS_AsyncServer.h
        SSL/HTTPS_ServerMultithreaded.cpp SSL/HTTPS_ServerMultithreaded.h
        SSL/HTTPS_Server_ThreadPool.cpp SSL/HTTPS_Server_ThreadPool.h
        Servers/EPollTCPServerContext.cpp
        Servers/EPollTCPServerContext.h
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
        ssh2
        rt
        crypt
        crypto
        ssl
)
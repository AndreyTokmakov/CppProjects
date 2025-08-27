cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Networking)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
add_compile_options(-c -Wall -O3 -std=c++26)
# add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

SET(OPENSSL_ROOT_DIR ${THIRD_PARTY_DIR}/openssl)

link_directories(${OPENSSL_ROOT_DIR})
include_directories("${OPENSSL_ROOT_DIR}/include")

include_directories(${THIRD_PARTY_DIR}/libssh2/include)
include_directories(${THIRD_PARTY_DIR}/libssh2/build/src)
link_directories(${THIRD_PARTY_DIR}/libssh2/build/src)

include_directories(${THIRD_PARTY_DIR}/grpc/include)
include_directories(${THIRD_PARTY_DIR}/abseil-cpp)
include_directories(${THIRD_PARTY_DIR}/json/include)

# find_package(gRPC CONFIG REQUIRED)
# message(STATUS "Using gRPC ${gRPC_VERSION}")

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        Common/Header.cpp
        APIServer/APIServer.cpp
        Common/HTTPHeader.cpp
        Common/HTTPRequest.cpp
        Common/HTTPResponse.cpp
        Common/URL.cpp
        Clients/Clients.cpp
        DNS/DNS_ResponseParser.cpp
        RawSockets/RawSocketsTests.cpp
        Experiments/Experiments.cpp
        NetLink/NetLink.cpp
        Utilities/Utilities.cpp
        RawSockets/BuildAndSendPacketsTests.cpp
        Servers/TestWebServer.cpp
        Experiments/UtilitiesTests.cpp
        Clients/UDPClient.cpp
        RawSockets/ARPTester.cpp
        Servers/SimpleServerSelect.cpp
        Servers/SocketTCPServer.cpp
        Servers/EPollTCPServer.cpp
        Servers/EPollTCPServerTwo.cpp
        Servers/EPollTCPServerEx.cpp
        Servers/EPollTCPServerDebug.cpp
        Servers/EPollTCPServerMultithreaded.cpp
        Servers/EPollTCPServerContext.cpp
        Servers/EPollTCPServerContextEx.cpp
        ServersTCP/AsynchTcpServerWithSessions.cpp

        ServerTcpAsynch/AsynchTcpService.cpp
        ServerTcpAsynch/Session.cpp
        ServerTcpAsynch/TcpServer.cpp

        RawSockets/CaptureTraffic.cpp
        PcapAnalyzer/PcapAnalyzer.cpp
        WiFi/WiFiExperiments.cpp
        ssh/SshExperiments.cpp
        SSL/SSL_Experiments.cpp
        SSL/TCP_SSL_Server.cpp
        SSL/HTTPS_Server.cpp
        SSL/HTTPS_AsyncServer.cpp
        SSL/HTTPS_ServerMultithreaded.cpp
        SSL/HTTPS_Server_ThreadPool.cpp
        Servers/EPollTCPServerContext.cpp
        # JRPC/JRPC.cpp
        SecureMessagingProxy/MessagingProxy.cpp
        UnixDomainSockets/UDSAsynchServer.cpp
        UnixDomainSockets/UDSAsynchServer_Base.cpp
        UnixDomainSockets/UDSAsynchServer_BufferPerSession.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/Utilities")
target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/Utilities")

target_link_libraries(${PROJECT_NAME}
        utils
        pthread
        ssh2
        rt
        crypt
        crypto
        ssl
)
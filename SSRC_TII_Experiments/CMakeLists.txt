cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(SSRC_TII_Experiments)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)


add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# LibSerial headers:
include_directories(../../third_party/libserial/src)
include_directories(../../third_party/libserial/src/libserial)
link_directories(../../third_party/libserial/build/lib)

# LibPCAP
# include_directories(/home/andtokm/DiskS/ProjectsUbuntu/third_party/libpcap)
# link_directories(/home/andtokm/DiskS/ProjectsUbuntu/third_party/libpcap/build)

# include all components
add_executable(${PROJECT_NAME} MACOSX_BUNDLE
    main.cpp
        Networking/Headers/ARPHeader.h
        Networking/Headers/EthernetHeader.h
        Networking/Headers/IPv4Header.h
        Networking/Headers/TCPHeader.h
        Networking/Headers/UDPHeader.h
        Networking/Headers/ICMPHeader.h
        Networking/Utils/Utilities.cpp
        Networking/Utils/Utilities.h
        Serial/LinuxSerial.cpp Serial/LinuxSerial.h
        Networking/Networking.cpp Networking/Networking.h
        Serial/SerialPort.h Serial/Exception.h
        Serial/SerialPort.cpp LibSerial/LibSerial.cpp
        LibSerial/LibSerial.h LibPcap/LibPcapExperiments.cpp
        LibPcap/LibPcapExperiments.h
        LibPcap/config.h
        WiFi/WiFiScanner.cpp WiFi/WiFiScanner.h ContikiDCube/ContikiDCube.cpp ContikiDCube/ContikiDCube.h)

# Link pthread library
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        -ldl
        libserial.a
        libpcap.a
)
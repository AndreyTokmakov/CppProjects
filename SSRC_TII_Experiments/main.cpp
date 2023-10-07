/**============================================================================
Name        : main.cpp
Created on  : 09.12.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : SSRC_TII_Experiments main cpp
============================================================================**/

#include <iostream>
#include <vector>
#include <string_view>

#include "Networking/Networking.h"
#include "LibPcap/LibPcapExperiments.h"
#include "WiFi/WiFiScanner.h"
#include "Serial/LinuxSerial.h"
#include "LibSerial/LibSerial.h"

// TODO:
// 1. Remove 'CalcChecksum' from TCPHeader .h file --> .cpp

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Networking::TestAll();

    // LibPcapExperiments::TestAll();
    // WiFiScanner::TestAll();

    LinuxSerial::TestAll();
    // LibSerial::TestAll();

    return EXIT_SUCCESS;
}

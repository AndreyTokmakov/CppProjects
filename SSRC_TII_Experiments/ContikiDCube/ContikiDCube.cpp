/**============================================================================
Name        : ContikiDCube.cpp
Created on  : 12.03.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ContikiDCube
============================================================================**/

#include "ContikiDCube.h"

#include <iostream>
#include <string_view>
#include <vector>
#include <cstdint>

namespace {
    constexpr size_t TB_MAX_SRC_DEST = 48;
    constexpr size_t TB_NUM_PATTERN = 1;
}

namespace ContikiDCube::Configuration
{
    struct Pattern
    {
        uint8_t traffic_pattern { 0 };                 // 0:unused, 1:p2p, 2:p2mp, 3:mp2p, 4: mp2mp
        uint8_t source_id[TB_MAX_SRC_DEST];            // Only source_id[0] is used for p2p/p2mp
        uint8_t destination_id[TB_MAX_SRC_DEST];       // Only destination_id[0] is used for p2p/mp2p
#if TESTBED_WITH_BORDER_ROUTER
        uint8_t br_id[TB_MAX_BR];                       // Default max is 16
#endif
        uint8_t msg_length { 0 };                       // Message length in bytes in/to EEPROM
        uint8_t msg_offsetH { 0 };                      // Message offset in bytes in EEPROM (high byte)
        uint8_t msg_offsetL { 0 };                      // Message offset in bytes in EEPROM (low byte)
        uint32_t periodicity { 0 };                     // Period in ms (0 indicates aperiodic traffic)
        uint32_t aperiodic_upper_bound { 0 };           // Upper bound for aperiodic traffic in ms
        uint32_t aperiodic_lower_bound { 0 };           // Lower bound for aperiodic traffic in ms
#if 1 //CONTIKI_TARGET_NRF52840
        uint32_t delta {};                              // The delay bound delta in ms
#endif
    };


    struct Config
    {
        uint8_t node_id {0};                // ID of the current node
        Pattern patterns[TB_NUM_PATTERN];   // Up to TB_NUMPATTERN parallel configurations
    };

    std::ostream& operator<<(std::ostream& stream, uint8_t v) {
        stream << static_cast<int>(v);
        return stream;
    }

    void printConfig(const Config& config)
    {
        std::cout << "node_id: " << config.node_id << std::endl;
        for (const Pattern& pattern: config.patterns)
        {
            std::cout << "  traffic_pattern: " << pattern.traffic_pattern << std::endl;

            std::cout << "    src_id:         { ";
            for (const auto& id: pattern.source_id)
                std::cout << id << ", ";
            std::cout << "}\n";

            std::cout << "    destination_id: { ";
            for (const auto& id: pattern.destination_id)
                std::cout << id << ", ";
            std::cout << "}\n";

            std::cout << "    msg_length : " << pattern.msg_length << std::endl;
            std::cout << "    msg_offsetH: " << pattern.msg_offsetH << std::endl;
            std::cout << "    msg_offsetL: " << pattern.msg_offsetL << std::endl;
            std::cout << "    periodicity: " << pattern.periodicity << std::endl;
            std::cout << "    aperiodic_upper_bound: " << pattern.aperiodic_upper_bound << std::endl;
            std::cout << "    aperiodic_lower_bound: " << pattern.aperiodic_lower_bound << std::endl;
            std::cout << "    delta: " << pattern.delta << std::endl;
        }
    }

    void Test()
    {
        // Config cfg {1, {{1, {0,1}, {0, 0, 1}}}};

        Config cfg {3,{
                {2,
                 {3,0,0,0},
                 {1,2,4,0},
                 8,
                 0,
                 0,
                 5000,
                 0,
                 0,
                 5000
                }
        }};
        printConfig(cfg);
    }
}

void ContikiDCube::TestAll([[maybe_unused]] const std::vector<std::string_view>& params)
{
    Configuration::Test();
}
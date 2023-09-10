//============================================================================
// Name        : Modules.cpp
// Created on  : 27.06.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Modules
//============================================================================

#include "Modules.h"

#include <iostream>
#include <charconv>
#include <fstream>
#include <string_view>
#include <vector>
#include <filesystem>

// INFO: https://losst.ru/fajlovaya-sistema-proc-v-linux#proclocks

namespace Modules
{
    static constexpr std::string_view modulesFile { "/proc/modules" };
    static constexpr std::string_view modulesFileDummy { "/proc/modules" };

    struct Module final {
        std::string name {};
        size_t size {0};
        uint16_t usedBy {0};
    };

    std::ostream& operator<<(std::ostream& stream, const Module& module) {
        stream << "Module("
               << module.name << ", "
               << module.size << ", "
               << module.usedBy << ")";
        return stream;
    }

    [[nodiscard]]
    Module extractModule(std::string_view line) {
        Module module {};

        auto pos1 = line.find(' ');
        if (std::string::npos != pos1) {
            module.name.assign(line, 0, pos1);
        } else return module;

        auto pos2 = line.find(' ', ++pos1);
        if (std::string::npos != pos2) {
            [[maybe_unused]]
            auto [ptr, ec] { std::from_chars(line.data() + pos1, line.data() + pos2, module.size) };
        } else return module;

        auto pos3 = line.find(' ', ++pos2);
        if (std::string::npos != pos3) {
            std::from_chars(line.data() + pos2, line.data() + pos3, module.usedBy);
        } else return module;

        return module;
    }

    [[nodiscard("")]]
    std::vector<std::string> readModulesFile() noexcept {
        std::vector<std::string> modules {};
        if (std::fstream file = std::fstream(modulesFileDummy.data(), std::fstream::in);
            file.is_open() && file.good()) {
            while (std::getline(file, modules.emplace_back())) { /** **/ }
        }
        return modules;
    }


    // INFO: Same as 'lsmod' command
    void ReadModulesFile()
    {
        std::vector<std::string> modules { readModulesFile() };
        for (const auto& mod: modules) {
            // std::cout << mod << std::endl;
            Module module {extractModule (mod)};
            std::cout << module << std::endl;
        }

    }

    [[nodiscard("This call may be expensive.")]]
    bool isModuleLoaded(std::string_view moduleName)
    {
        std::string line {};
        if (std::fstream file = std::fstream(modulesFileDummy.data(), std::fstream::in);
                file.is_open() && file.good()) {
            while (std::getline(file, line)) { /* */
                if (const auto pos = line.find(' '); std::string::npos != pos) {
                    const int res = line.compare(0, pos, moduleName.data(), moduleName.size());
                    if (0 == res)
                        return true;
                }
            }
            return false;
        }
        return false;
    }

    void CheckLoadedModules() {
        std::cout << std::boolalpha << isModuleLoaded("nvme_core") << std::endl;
    }



    void ParseModuleLine() {
        const std::string line { "videobuf2_common_ex 614402 41 uvcvideo,videobuf2_vmalloc,videobuf2_memops,"
                                 "videobuf2_v4l2, Live 0x0000000000000000"};

        Module module {extractModule (line)};
        std::cout << module << std::endl;


    }
};

void Modules:: TestAll() {
    ReadModulesFile();

    // CheckLoadedModules();

    // ParseModuleLine();
};

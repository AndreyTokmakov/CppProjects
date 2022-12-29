//============================================================================
// Name        : main.cpp
// Created on  : 07.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Rapid JSON C++ src
//============================================================================

#include <iostream>
#include <memory>
#include <math.h>
#include <algorithm>

#include <thread>
#include <future>

#include <vector>
#include <algorithm>
#include <array>
#include <set>
#include <string>
#include <string_view>
#include <mutex>
#include <atomic>
#include <numeric>
#include <fstream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"

namespace SimpleTests 
{
    void ReadAndParse() {
        constexpr std::string_view jsonFile {R"(/home/andtokm/Projects/data/cases/2878/automodeling/2878_session.json)"};

        rapidjson::Document document;
        if (std::ifstream ifs(jsonFile.data()); ifs.is_open() && ifs.good()) {\
            rapidjson::IStreamWrapper isw(ifs);
            document.ParseStream(isw);
            // Can be removed: filebuf object is RAII 
            ifs.close();
        }

        if (document.IsNull() || !document.IsObject()) {
            std::cout << "Failed to read document " << jsonFile << std::endl;
            return;
        }

        constexpr std::string_view tagParamerName {"hash_for_patient_tag"};
        if (document.HasMember(tagParamerName.data()) && document[tagParamerName.data()].IsString()) {
            const std::string patientTag = document[tagParamerName.data()].GetString();
            std::cout << tagParamerName << " = " << patientTag << std::endl;

        }
    }
}


int main([[maybe_unused]] int argc,
		 [[maybe_unused]] char** argv) 
{
    SimpleTests::ReadAndParse();

    return 0;
}
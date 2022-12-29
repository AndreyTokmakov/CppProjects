//============================================================================
// Name        : TreatmentPlan_Tests.cpp
// Created on  : 18.08.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Treatment plan src
//============================================================================

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <filesystem>
#include <string>
#include <string_view>
#include <sstream>
#include <numeric>

#include "../Utilities/Utilities.h"
#include "TreatmentPlan.h"

namespace {
    constexpr std::string_view TREATMENT_PLAN_JSON_FILE {
            R"(/home/andtokm/Projects/data/cases/2280/Plan.json)" };

}

namespace TreatmentPlan_Tests {

    using namespace TreatmentPlan;

    template<typename T>
    std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vect) {
        std::cout << "[" << std::setprecision(12);
        for (const T& entry : vect)
            stream << entry << ' ';
        std::cout << "]";
        return stream;
    }

    template<typename _Ty, size_t _Size>
    std::ostream& operator<<(std::ostream& stream, const std::array<_Ty, _Size>& data) {
        std::cout << "[" << std::setprecision(12);
        for (const _Ty& entry : data)
            stream << entry << ' ';
        std::cout << "]";
        return stream;
    }


    //-----------------------------------------------------------------------------------

    void ModellingDataTest() {
        const Plan plan = Parser::Parse(TREATMENT_PLAN_JSON_FILE);
        for (const auto& [toothId, toothData] : plan.modellingData.tooth) {
            std::cout << "   origin: " << toothData.origin << std::endl;
        }
    }


    void ModellingDataTest_Axis() {
        const Plan plan = Parser::Parse(TREATMENT_PLAN_JSON_FILE);
        std::cout << "Axex:\n";
        for (const auto& [toothId, toothData] : plan.modellingData.tooth) {
            std::cout << "   " << toothId << ": " << toothData.axes << std::endl;
        }
    }

    void ModellingDataTest_Quaternions() {
        const Plan plan = Parser::Parse(TREATMENT_PLAN_JSON_FILE);
        std::cout << "Quaternions:\n";
        for (const auto& [toothId, toothData] : plan.modellingData.tooth) {
            std::cout << "   " << toothId << ": "
                    << toothData.quaternion.x() << ",  "
                    << toothData.quaternion.y() << ",  "
                    << toothData.quaternion.z() << ",  "
                    << toothData.quaternion.w() << std::endl;
        }
    }

    void KeyframesTest()
    {
        const Plan plan = Parser::Parse(TREATMENT_PLAN_JSON_FILE);

        std::cout << "Lower:" << std::endl;
        for (const auto& lowerKeyFrames : plan.keyframes.lower) {
            std::cout << "==================" << std::endl;
            for (const auto& [id, data] : lowerKeyFrames.toothFrames) {
                std::cout << "  " << id << "\n      quaternions:" << std::endl;
                for (const Quaternion& q : data.quaternions)
                    std::cout << "          " << q << std::endl;
                std::cout << "      quaternion_index: " << data.quaternion_index << std::endl;
                std::cout << "      translation: " << data.translation << std::endl;
            }
        }

        std::cout << "Upper:" << std::endl;
        for (const auto& keyFrames : plan.keyframes.upper) {
            std::cout << "==================" << std::endl;
            for (const auto& [id, data] : keyFrames.toothFrames) {
                std::cout << "  " << id << "\n      quaternions:" << std::endl;
                for (const Quaternion& q : data.quaternions)
                    std::cout << "          " << q << std::endl;
                std::cout << "      quaternion_index: " << data.quaternion_index << std::endl;
                std::cout << "      translation: " << data.translation << std::endl;
            }
        }
    }

    void GetFinalPointsTest()
    {
        const Plan& plan = Parser::Parse(TREATMENT_PLAN_JSON_FILE);
        const std::map<uint16_t, Point3d>& points = plan.getPointsFinal();
        for (const auto& [id, pt] : points) {
            std::cout << "[" << pt[0] << ", " << pt[1] << ", " << pt[2] << "]," << std::endl;
        }
    }

    std::string pointToString(const Point3d& point) {
        std::stringstream sstream;
        for (int i = 0; i < 3; ++i)
            sstream << std::setprecision(16) << point[i] << ",";

        std::string text{ "[" };
        text.append(sstream.str());
        text.back() = ']';
        return text;
    }

    void RemoveQuotesFromFile(std::string_view inputFile, std::string_view ouputFile) {
        std::vector<std::string> inputLines, outputLines;
        std::fstream inFile(inputFile.data());

        while (std::getline(inFile, inputLines.emplace_back())) { /** Read file lines **/ }
        inFile.close();

        // Replace "[....]" ---> [....]
        for (auto&& s : inputLines) {
            if (auto start = s.find(R"("[)"); std::string::npos != start) {
                if (auto end = s.find(R"(]")", start); std::string::npos != end) {
                    s.erase(s.begin() + start);
                    s.erase(s.begin() + end);
                }
            }
            outputLines.emplace_back(s);
        }

        std::fstream outFile(ouputFile.data(), std::ios::out);
        for (auto s : outputLines)
            outFile << s << '\n';
        outFile.close();
    }
}

void TreatmentPlan_Tests::TestAll()
{
    // ModellingDataTest();
    // ModellingDataTest_Axis();
    ModellingDataTest_Quaternions();

    // KeyframesTest();
    // GetFinalPointsTest();
}
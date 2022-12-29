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
#include <sstream>
#include <numeric>

#include "../Utilities.h"
#include "../Types.h"
#include "TreatmentPlan.h"

namespace {
    constexpr std::string_view TREATMENT_PLAN_JSON_FILE {
        R"(/home/andtokm/Projects/data/cases/2333/Treatment plan_01_2021-02-18-22:40:50.json)" };
    constexpr std::string_view ML_CASES_FOLDER {
        R"(/home/andtokm/Projects/data/cases_ml)" };

    constexpr std::string_view JSON_OUT_TEST_FILE_BEFORE {
        R"(/home/andtokm/Projects/CppProjects/AutoModelingTools/TreatmentPlan/data/out_before.json)" };
    constexpr std::string_view JSON_OUT_RESULT_FILE_BEFORE {
            R"(/home/andtokm/Projects/CppProjects/AutoModelingTools/TreatmentPlan/data/out_before_res.json)" };

    constexpr std::string_view JSON_OUT_TEST_FILE_AFTER {
            R"(/home/andtokm/Projects/CppProjects/AutoModelingTools/TreatmentPlan/data/out_after.json)" };
    constexpr std::string_view JSON_OUT_RESULT_FILE_AFTER {
            R"(/home/andtokm/Projects/CppProjects/AutoModelingTools/TreatmentPlan/data/out_after_res.json)" };
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

    std::map<uint16_t, Point3d> getOriginPointsUpper(const TreatmentPlan::Plan& plan) noexcept {
        std::map<uint16_t, Point3d> points;
        for (const auto& [toothId, toothData]: plan.modellingData.tooth) {
            if (Utilities::isUpperTooth(toothId))
                points.emplace(toothId, toothData.origin);
        }
        return points;
    }

    std::map<uint16_t, Point3d> getOriginPointsFromFile_Upper(std::string_view filePath) noexcept {
        const TreatmentPlan::Plan& plan = TreatmentPlan::Parser::Parse(filePath);
        std::map<uint16_t, Point3d> points;
        for (const auto& [toothId, toothData]: plan.modellingData.tooth) {
            if (Utilities::isUpperTooth(toothId))
                points.emplace(toothId, toothData.origin);
        }
        return points;
    }

    std::map<uint16_t, Point3d> getFinalPointsUpper(const TreatmentPlan::Plan& plan) noexcept {
        std::map<uint16_t, Point3d> points;
        for (const auto& [toothId, point]: plan.getPointsFinal()) {
            if (Utilities::isUpperTooth(toothId))
                points.emplace(toothId, point);
        }
        return points;
    }

    std::map<uint16_t, Point3d> getFinalPointsFromFile_Upper(std::string_view filePath) noexcept {
        const TreatmentPlan::Plan& plan = TreatmentPlan::Parser::Parse(filePath);
        std::map<uint16_t, Point3d> points;
        for (const auto& [toothId, point]: plan.getPointsFinal()) {
            if (Utilities::isUpperTooth(toothId))
                points.emplace(toothId, point);
        }
        return points;
    }

    /*
    std::vector<Point3d> GetAllPoints_Upper() {
        std::vector<Point3d> points;
        points.reserve(1000);
        for(auto const& dir_entry: std::filesystem::directory_iterator{ML_CASES_FOLDER}) {
            if (dir_entry.is_directory()) {
                for(auto const& entry: std::filesystem::directory_iterator{dir_entry}) {
                    if (entry.is_regular_file() && entry.path().has_extension() &&
                        0 == entry.path().extension().compare(".json")) {
                        const std::map<uint16_t, Point3d>& pts = getFinalPointsFromFile_Upper(entry.path().c_str());
                        points.insert(points.end(), pts.begin(), pts.end());
                    }
                }
            }
        }
        return points;
    }
    */

    // Return std::vector of Points
    std::vector<std::map<uint16_t, Point3d>> GetPointsVectors_Upper() {
        std::vector<std::map<uint16_t, Point3d>> pointsMapVector;
        for(auto const& dir_entry: std::filesystem::directory_iterator{ML_CASES_FOLDER}) {
            if (dir_entry.is_directory()) {
                for(auto const& entry: std::filesystem::directory_iterator{dir_entry}) {
                    if (entry.is_regular_file() && entry.path().has_extension() &&
                        0 == entry.path().extension().compare(".json")) {
                        pointsMapVector.emplace_back(getFinalPointsFromFile_Upper(entry.path().c_str()));
                    }
                }
            }
        }
        return pointsMapVector;
    }

    //-----------------------------------------------------------------------------------

    void ModellingDataTest() {
        const Plan plan = Parser::Parse(TREATMENT_PLAN_JSON_FILE);
        for (const auto& [toothId, toothData]: plan.modellingData.tooth) {
            std::cout << "   origin: " << toothData.origin << std::endl;
        }
    }

    void KeyframesTest()
    {
        const Plan plan = Parser::Parse(TREATMENT_PLAN_JSON_FILE);

        std::cout << "Lower:" << std::endl;
        for (const auto& lowerKeyFrames: plan.keyframes.lower) {
            std::cout << "=================="<< std::endl;
            for (const auto& [id, data] : lowerKeyFrames.toothFrames) {
                std::cout << "  " << id << "\n      quaternions:"<< std::endl;
                for (const Quaternion& q: data.quaternions)
                    std::cout << "          " << q << std::endl;
                std::cout << "      quaternion_index: " << data.quaternion_index << std::endl;
                std::cout << "      translation: "      << data.translation << std::endl;
            }
        }

        std::cout << "Upper:" << std::endl;
        for (const auto& keyFrames: plan.keyframes.upper) {
            std::cout << "=================="<< std::endl;
            for (const auto& [id, data] : keyFrames.toothFrames) {
                std::cout << "  " << id << "\n      quaternions:"<< std::endl;
                for (const Quaternion& q: data.quaternions)
                    std::cout << "          " << q << std::endl;
                std::cout << "      quaternion_index: " << data.quaternion_index << std::endl;
                std::cout << "      translation: "      << data.translation << std::endl;
            }
        }
    }

    void GetFinalPointsTest()
    {
        const Plan& plan = Parser::Parse(TREATMENT_PLAN_JSON_FILE);
        const std::map<uint16_t, Point3d>& points = plan.getPointsFinal();
        for (const auto& [id, pt]: points) {
            std::cout << "[" << pt[0] << ", " << pt[1] << ", " << pt[2] << "]," << std::endl;
        }
    }

    std::string pointToString(const Point3d& point) {
        std::stringstream sstream;
        for (int i = 0; i < 3; ++i)
            sstream << std::setprecision(16) << point[i] << ",";

        std::string text {"["};
        text.append(sstream.str());
        text.back() = ']';
        return text;
    }

    void RemoveQuotesFromFile(std::string_view inputFile, std::string_view ouputFile) {
        std::vector<std::string> inputLines, outputLines;
        std::fstream inFile (inputFile.data());

        while (std::getline(inFile, inputLines.emplace_back())) { /** Read file lines **/ }
        inFile.close();

        // Replace "[....]" ---> [....]
        for (auto&& s: inputLines) {
            if (auto start = s.find(R"("[)"); std::string::npos != start) {
                if (auto end = s.find(R"(]")", start); std::string::npos != end) {
                    s.erase(s.begin() + start);
                    s.erase(s.begin() + end);
                }
            }
            outputLines.emplace_back(s);
        }

        std::fstream outFile (ouputFile.data(), std::ios::out);
        for (auto s: outputLines)
            outFile << s << '\n';
        outFile.close();
    }

    bool isAllRequiredTeethPresent_Upper(const std::map<uint16_t, Point3d>& pointsMap) {
        for (uint16_t id: {11, 12, 13, 14, 15, 16,  21, 22, 23, 24, 25, 26} ) {
            if (false == pointsMap.contains(id)) {
                std::cout << "Tooth " << id << " is missing\n";
                return false;
            }
        }
        return true;
    }

    void WriteOriginPoints_FromML_Cases_ToFile()
    {
        boost::property_tree::ptree root;
        for(auto const& dir_entry: std::filesystem::directory_iterator{ML_CASES_FOLDER}) {
            if (dir_entry.is_directory()) {
                for(auto const& entry: std::filesystem::directory_iterator{dir_entry}) {
                    if (entry.is_regular_file() && entry.path().has_extension() &&
                        0 == entry.path().extension().compare(".json"))
                    {
                        std::string caseId(dir_entry.path().filename());
                        const std::map<uint16_t, Point3d>& ptsMap = getOriginPointsFromFile_Upper(entry.path().c_str());
                        if (false == isAllRequiredTeethPresent_Upper(ptsMap)) {
                            std::cout << "Skipping case " << caseId << std::endl;
                            continue;
                        }

                        auto& pointsNode = root.add_child(caseId,  boost::property_tree::ptree());
                        for (const auto& [id, pts]: ptsMap) {
                            pointsNode.add(std::to_string(id), pointToString(pts));
                        }
                    }
                }
            }
        }
        boost::property_tree::write_json(JSON_OUT_TEST_FILE_BEFORE.data(), root);
        RemoveQuotesFromFile(JSON_OUT_TEST_FILE_BEFORE, JSON_OUT_RESULT_FILE_BEFORE);
    }

    void WriteFinalPoints_FromML_Cases_ToFile()
    {
        boost::property_tree::ptree root;
        for(auto const& dir_entry: std::filesystem::directory_iterator{ML_CASES_FOLDER}) {
            if (dir_entry.is_directory()) {
                for(auto const& entry: std::filesystem::directory_iterator{dir_entry}) {
                    if (entry.is_regular_file() && entry.path().has_extension() &&
                        0 == entry.path().extension().compare(".json"))
                    {
                        const std::map<uint16_t, Point3d>& ptsMap = getFinalPointsFromFile_Upper(entry.path().c_str());
                        std::string caseId(dir_entry.path().filename());
                        auto& pointsNode = root.add_child(caseId,  boost::property_tree::ptree());
                        for (const auto& [id, pt]: ptsMap) {
                            pointsNode.add(std::to_string(id), pointToString(pt));
                        }
                    }
                }
            }
        }
        boost::property_tree::write_json(JSON_OUT_TEST_FILE_AFTER.data(), root);
        RemoveQuotesFromFile(JSON_OUT_TEST_FILE_AFTER, JSON_OUT_RESULT_FILE_AFTER);
    }
}

void TreatmentPlan_Tests::TestAll()
{
    // ModellingDataTest();
    // KeyframesTest();
    // GetFinalPointsTest();

    WriteOriginPoints_FromML_Cases_ToFile();
    WriteFinalPoints_FromML_Cases_ToFile();
}
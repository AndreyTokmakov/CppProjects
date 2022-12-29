//============================================================================
// Name        : TreatmentPlan.cpp
// Created on  : 18.08.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Automodeling Configuration parser
//============================================================================

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <map>
// #include <concepts>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "../Utilities.h"
#include "../Types.h"
#include "TreatmentPlan.h"

namespace TreatmentPlan
{
    std::map<uint16_t, Point3d> Plan::getPointsFinal() const noexcept {
        const std::map<uint16_t, ToothFrame>& lowerFrames = keyframes.lower.back().toothFrames;
        const std::map<uint16_t, ToothFrame>& upperFrames = keyframes.upper.back().toothFrames;

        std::map<uint16_t, Point3d> points;
        for (const auto& [toothId, toothData]: modellingData.tooth) {
            const std::map<uint16_t, ToothFrame>& frames =
                    Utilities::isLowerTooth(toothId) ? lowerFrames: upperFrames;
            auto [iter, status] = points.emplace(toothId, toothData.origin);
            if (const auto toothFrame = frames.find(toothId); frames.end() != toothFrame) {
                iter->second = iter->second + toothFrame->second.translation;
            }
        }
        return points;
    }
}

namespace TreatmentPlan::Parser
{
    constexpr std::string_view JAW_SIDE_TYPE_LOWER_PARAM { "lower" };
    constexpr std::string_view JAW_SIDE_TYPE_UPPER_PARAM { "upper" };

    constexpr std::string_view QUATERNIONS_PARAM { "quaternions" };
    constexpr std::string_view TRANSLATION_PARAM { "translation" };
    constexpr std::string_view QUATERNIONS_INDEX_PARAM { "quaternion_index" };

    constexpr std::string_view BASE_PARAM { "base" };
    constexpr std::string_view STATS_PARAM { "stats" };
    constexpr std::string_view MATRIX_PARAM { "matrix" };
    constexpr std::string_view MATRICES_PARAM { "matrices" };
    constexpr std::string_view EXCURSION_PARAM { "excursion" };
    constexpr std::string_view KEYFRAMES_PARAM { "keyframes" };
    constexpr std::string_view POINT_MAP_PARAM { "point_map" };
    constexpr std::string_view MARKUP_DATA_PARAM { "markupData" };
    constexpr std::string_view VISIBILITY_PARAM { "visibility" };
    constexpr std::string_view ATTACHMENTS_PARAM { "attachments" };
    constexpr std::string_view SEPARATIONS_PARAM { "separations" };
    constexpr std::string_view MODELLING_DATA_PARAM { "modellingData" };
    constexpr std::string_view REMOVED_ROOTS_PARAM { "removed_roots" };
    constexpr std::string_view STEP_MATRICES_PARAM { "step_matrices" };
    constexpr std::string_view MISSING_CROWNS_PARAM { "missing_crowns" };
    constexpr std::string_view SAME_KEYFRAMES_PARAM { "same_keyframes" };
    constexpr std::string_view STEP_KEYFRAMES_PARAM { "step_keyframes" };
    constexpr std::string_view FAKE_ROOTS_DATA_PARAM { "fake_roots_data" };
    constexpr std::string_view MANUAL_NUM_STEPS_PARAM { "manual_num_steps" };

    constexpr std::string_view MODELING_DATA_AXES_PARAM { "axes" };
    constexpr std::string_view MODELING_DATA_ORIGIN_PARAM { "origin" };
    constexpr std::string_view MODELING_DATA_QUATERNION_PARAM { "quaternion" };
    constexpr std::string_view MODELING_DATA_ANCHOR_POINT_1_PARAM { "anchor_point_1" };
    constexpr std::string_view MODELING_DATA_ANCHOR_POINT_2_PARAM { "anchor_point_2" };
    constexpr std::string_view MODELING_DATA_ANCHOR_POINT_3_PARAM { "anchor_point_3" };
    constexpr std::string_view MODELING_DATA_ANCHOR_POINT_LOCAL_1_PARAM { "anchor_point_local_1" };
    constexpr std::string_view MODELING_DATA_ANCHOR_POINT_LOCAL_2_PARAM { "anchor_point_local_2" };
    constexpr std::string_view MODELING_DATA_ANCHOR_POINT_LOCAL_3_PARAM { "anchor_point_local_3" };

    /*
    template<typename _Ty>
    concept PTreeExtractable = requires(_Ty val, boost::property_tree::ptree ptree) {
        { _Ty{} };
        { ptree.get_value<_Ty>() };
        { std::cout << _Ty{} };
    };
    */

    template<typename _Ty>
    std::vector<_Ty> extractList(const boost::property_tree::ptree& node) {
        std::vector<_Ty> result;
        result.reserve(node.size());
        for (const auto& value : node)
            result.emplace_back(value.second.get_value<_Ty>());
        return result;
    }

    template<typename _Ty>
    void extractList(const boost::property_tree::ptree& node, std::vector<_Ty>& result) {
        result.reserve(node.size());
        for (const auto& value : node)
            result.emplace_back(value.second.get_value<_Ty>());
    }

    template<typename _Ty, size_t _Size>
    void extractArray(const boost::property_tree::ptree& node, std::array<_Ty, _Size>& result) {
        for (size_t index {0}; const auto& value : node) {
            // TODO: Assert size
            result[index++] = value.second.get_value<_Ty>();
        }
    }

    template<typename _Ty, size_t _Size>
    void extractPoint(const boost::property_tree::ptree& node, Types::Point<_Size, _Ty>& point) {
        for (size_t index {0}; const auto& value : node) {
            // TODO: Assert size
            point[index++] = value.second.get_value<_Ty>();
        }
    }

    template<typename _Ty, size_t _Size>
    std::array<_Ty, _Size> extractArray(const boost::property_tree::ptree& node) {
        std::array<_Ty, _Size> result;
        for (size_t index {0}; const auto& value : node) {
            // TODO: Assert size
            result[index++] = value.second.get_value<_Ty>();
        }
        return result;
    }

    namespace pt = boost::property_tree;
    using PTree = pt::ptree;

    //===============================================================================================//

    void parseModellingAsex(const PTree& node, TreatmentPlan::Axes& axes) {
        extractList(node, axes);
    }

    void parseModellingOrigin(const PTree& node, [[maybe_unused]] TreatmentPlan::Origin& origin) {
        // TODO: refactor
        const std::vector<double>& data = extractList<double>(node);
        origin.assign(data);
    }

    void parseModellingQuaternion(const PTree& node, TreatmentPlan::Quaternion& quaternion) {
        extractArray(node, quaternion);
    }

    void parseModellingAnchorPointOne(const PTree& node, TreatmentPlan::Point3d& point) {
        // TODO: refactor
        const std::vector<double>& data = extractList<double>(node);
        point.assign(data);
    }

    void parseModellingAnchorPointTwo(const PTree& node, TreatmentPlan::Point3d& point) {
        // TODO: refactor
        const std::vector<double>& data = extractList<double>(node);
        point.assign(data);
    }

    void parseModellingAnchorPointThree(const PTree& node, TreatmentPlan::Point3d& point) {
        // TODO: refactor
        const std::vector<double>& data = extractList<double>(node);
        point.assign(data);
    }

    void parseModellingAnchorPointOneLocal(const PTree& node, TreatmentPlan::Point3d& point) {
        extractPoint<double, 3>(node, point);
    }

    void parseModellingAnchorPointTwoLocal(const PTree& node, TreatmentPlan::Point3d& point) {
        extractPoint<double, 3>(node, point);
    }

    void parseModellingAnchorPointThreeLocal(const PTree& node, TreatmentPlan::Point3d& point) {
        extractPoint<double, 3>(node, point);
    }

    void parseModellingData(const PTree& node, TreatmentPlan::Plan& plan) {
        for (const auto& [toothId, toothNode] : node.get_child("")) {
            auto [iter, status] = plan.modellingData.tooth.emplace(atoi(toothId.data()), ToothModellingData{});
            ToothModellingData& toothData = iter->second;
            for (const auto& [name, value] : toothNode.get_child("")) {
                if (0 == name.compare(MODELING_DATA_AXES_PARAM))
                    parseModellingAsex(value, toothData.axes);
                else if (0 == name.compare(MODELING_DATA_ORIGIN_PARAM))
                    parseModellingOrigin(value, toothData.origin);
                else if (0 == name.compare(MODELING_DATA_QUATERNION_PARAM))
                    parseModellingQuaternion(value, toothData.quaternion);
                else if (0 == name.compare(MODELING_DATA_ANCHOR_POINT_1_PARAM))
                    parseModellingAnchorPointOne(value, toothData.anchor_point_1);
                else if (0 == name.compare(MODELING_DATA_ANCHOR_POINT_2_PARAM))
                    parseModellingAnchorPointTwo(value, toothData.anchor_point_2);
                else if (0 == name.compare(MODELING_DATA_ANCHOR_POINT_3_PARAM))
                    parseModellingAnchorPointThree(value, toothData.anchor_point_3);
                else if (0 == name.compare(MODELING_DATA_ANCHOR_POINT_LOCAL_1_PARAM))
                    parseModellingAnchorPointOneLocal(value, toothData.anchor_point_local_1);
                else if (0 == name.compare(MODELING_DATA_ANCHOR_POINT_LOCAL_2_PARAM))
                    parseModellingAnchorPointTwoLocal(value, toothData.anchor_point_local_2);
                else if (0 == name.compare(MODELING_DATA_ANCHOR_POINT_LOCAL_3_PARAM))
                    parseModellingAnchorPointThreeLocal(value, toothData.anchor_point_local_3);
            }
        }
    }

    void parseToothFrame(const PTree& node, ToothFrame& toothFrame) {
        for (const auto& [name, data] : node.get_child("")) {
            if (0 == name.compare(QUATERNIONS_PARAM)) {
                for (const auto& [_, quatData] : data.get_child("")) {
                    extractArray<double, 4>(quatData, toothFrame.quaternions.emplace_back());
                }
            } else if(0 == name.compare(TRANSLATION_PARAM)) {
                toothFrame.translation.assign(extractList<double>(data));
            } else if(0 == name.compare(QUATERNIONS_INDEX_PARAM)) {
                toothFrame.quaternion_index  = data.get_value<size_t>();
            } else {
                // TODO: Report error
            }
        }
    }

    void parseTeethKeyframes(const PTree& node, Keyframe& keyframe) {
        for (const auto& [toothId, frameData] : node.get_child("")) {
            auto [iter, status] = keyframe.toothFrames.emplace(atoi(toothId.data()), ToothFrame{});
            parseToothFrame(frameData, iter->second);
        }
    }

    void parseJawSideKeyframes(const PTree& node, std::vector<Keyframe>& jawSideKeyframes) {
        for (const auto& [_, keyframes] : node.get_child("")) {
            parseTeethKeyframes(keyframes, jawSideKeyframes.emplace_back());
        }
    }

    void parseKeyframes(const PTree& node, TreatmentPlan::Plan& plan) {
        for (const auto& [side, keyframes] : node.get_child("")) {
            if (0 == side.compare(JAW_SIDE_TYPE_LOWER_PARAM)) {
                parseJawSideKeyframes(keyframes, plan.keyframes.lower);
            } else if(0 == side.compare(JAW_SIDE_TYPE_UPPER_PARAM)) {
                parseJawSideKeyframes(keyframes, plan.keyframes.upper);
            } else {
                // TODO: Report error
            }
        }
    }

    void parseOther([[maybe_unused]] const PTree& node) {
        // std::cout << "Parsing 'OTHERS' block\n";
    }

    // TODO: check for RNVO
    //       Performance reasons
    [[nodiscard]]
    TreatmentPlan::Plan Parse(std::string_view filePath) {
        boost::property_tree::ptree root;
        boost::property_tree::read_json(filePath.data(), root);

        TreatmentPlan::Plan plan;
        for (const auto& [name, value] : root.get_child("")) {
            if (0 == name.compare(MODELLING_DATA_PARAM))
                parseModellingData(value, plan);
            else if (0 == name.compare(KEYFRAMES_PARAM))
                parseKeyframes(value, plan);
            else
                parseOther(value);
        }
        return plan;
    }
}


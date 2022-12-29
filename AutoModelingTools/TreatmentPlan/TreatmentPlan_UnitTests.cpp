//============================================================================
// Name        : TreatmentPlan_UnitTests.cpp
// Created on  : 18.08.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Treatment plan unit-src
//============================================================================

#include <exception>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "TreatmentPlan.h"

#define ASSERT(expr) \
	if (false == (expr)) { \
        std::cout << "Error at " << __FUNCTION__ << "." << __LINE__ << std::endl; \
        throw std::runtime_error("Test failed.");                           \
    }

#define ASSERT_NOT(expr) \
	if (true == (expr)) {\
        std::cout << "Error at " << __FUNCTION__ << "." << __LINE__ << std::endl; \
        throw std::runtime_error("Test failed.");                           \
    }

namespace {
    constexpr std::string_view JSON_PLAN_FILE {
        R"(/home/andtokm/Projects/CppProjects/AutoModelingTools/TreatmentPlan/data/Plan.json)"
    };
}

namespace TreatmentPlan_UnitTests::ModellingDataTests
{
    void TestSize(const TreatmentPlan::Plan& plan) {
        ASSERT(24 == plan.modellingData.tooth.size());
    }

    void AnchorPoint_1_Test(const TreatmentPlan::Plan& plan) {
        std::map<uint16_t, ToothModellingData> toothMap = plan.modellingData.tooth;
        const auto& tooth11 = toothMap[11];
        const auto& point = tooth11.anchor_point_1;
        ASSERT(1.25 == point[0]);
        ASSERT(3.24 == point[1]);
        ASSERT(6.123 == point[2]);
    }
}

void TreatmentPlan_UnitTests::TestAll(){
    const TreatmentPlan::Plan& plan = TreatmentPlan::Parser::Parse(JSON_PLAN_FILE);

    ModellingDataTests::TestSize(plan);
    ModellingDataTests::AnchorPoint_1_Test(plan);
}
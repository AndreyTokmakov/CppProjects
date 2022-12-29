//============================================================================
// Name        : SetUpTorksAndPositions.h
// Created on  : 13.01.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : SetUpTorksAndPositions
//============================================================================

#ifndef CPPPROJECTS_SETUPTORKSANDPOSITIONS_H
#define CPPPROJECTS_SETUPTORKSANDPOSITIONS_H

#include "../Model/Tooth.h"

namespace SetUpTorksAndPositions {
    void TestAll(const std::vector<std::string_view>& params);

    void alignTeethVertical(std::unordered_map<unsigned short, Model::Tooth>& teethMap);
    void alignTeethTorks(std::unordered_map<unsigned short, Model::Tooth>& teethMap);
};

#endif //CPPPROJECTS_SETUPTORKSANDPOSITIONS_H

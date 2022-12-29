//============================================================================
// Name        : Experiments.h
// Created on  : 21.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Automodeling experiments features
//============================================================================

#ifndef AUTOMODELING_EXPERIMENTS__INCLUDE_GUARD
#define AUTOMODELING_EXPERIMENTS__INCLUDE_GUARD

#include <string_view>
#include <vector>

namespace Experiments {
    void TestAll([[maybe_unused]] const std::vector<std::string_view>& params);
}

#endif //!AUTOMODELING_EXPERIMENTS__INCLUDE_GUARD
//============================================================================
// Name        : Utilities.cpp
// Created on  : 26.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Points
//============================================================================

#include "Utilities.h"

namespace Utilities {

    bool isLowerTooth([[maybe_unused]] uint_fast16_t id) {
        return id >= MIN_LOWER_JAW_TOOTH && id <= MAX_LOWER_JAW_TOOTH && (20 != id && 19 != id);
    }

    bool isUpperTooth([[maybe_unused]] uint_fast16_t id) {
        return id >= MIN_UPPER_JAW_TOOTH && id <= MAX_UPPER_JAW_TOOTH && (40 != id && 39 != id);
    }
}
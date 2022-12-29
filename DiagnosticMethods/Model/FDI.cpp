//============================================================================
// Name        : FDI.cpp
// Created on  : 10.12.2021.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : FDI
//============================================================================

#include "FDI.h"

[[nodiscard("Make sure to handle return value")]]
bool FDI::isLowerTooth(FDI::IdType id) noexcept {
    return id >= MIN_LOWER_JAW_TOOTH && id <= MAX_LOWER_JAW_TOOTH && (20 != id && 19 != id);
}

[[nodiscard("Make sure to handle return value")]]
bool FDI::isUpperTooth(FDI::IdType id) noexcept {
    return id >= MIN_UPPER_JAW_TOOTH && id <= MAX_UPPER_JAW_TOOTH && (40 != id && 39 != id);
}

[[nodiscard("Make sure to handle return value")]]
bool FDI::isLeftSideTooth(FDI::IdType id) noexcept {
    return (id >= 21 && id <= 28) || (id >= 31 && id <= 38);
}

[[nodiscard("Make sure to handle return value")]]
bool FDI::isRightSideTooth(FDI::IdType id) noexcept {
    return (id >= 11 && id <= 28) || (id >= 41 && id <= 48);
}
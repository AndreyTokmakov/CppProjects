//============================================================================
// Name        : DiagnosticsMethods.h
// Created on  : 29.09.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Diagnostics methods
//============================================================================

#ifndef AUTOMODELINGEXPERIMENTS_DIAGNOSTICSMETHODS_H
#define AUTOMODELINGEXPERIMENTS_DIAGNOSTICSMETHODS_H

#include <string_view>
#include <vector>

namespace DiagnosticsMethods {
    void TestAll([[maybe_unused]] const std::vector<std::string_view>& params);
}

#endif // !AUTOMODELINGEXPERIMENTS_DIAGNOSTICSMETHODS_H
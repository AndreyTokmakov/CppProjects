//============================================================================
// Name        : Environment.h
// Created on  : 07.06.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Environment
//============================================================================

#include "Environment.h"

#include <iostream>
#include <string_view>
#include <optional>


namespace Environment
{
    std::optional<std::string> getEnvironmentVariable(std::string_view variable) {
        if (const char* env_p = std::getenv(variable.data()))
            return env_p;
        return std::nullopt;
    }

    void setVariable() {
        constexpr std::string_view name {"MY_TEST_VAR"};
        std::string var {"MY_TEST_VAR=/my/new/temp/path/"};

        std::cout << name << " = " << getEnvironmentVariable(name).value_or("(None)") << std::endl;
        putenv( var.data() );
        std::cout << name << " = " << getEnvironmentVariable(name).value_or("(None)") << std::endl;
    }

    void EnvironmentVariable() {
        std::cout << getEnvironmentVariable("PWD").value_or("(none)") << std::endl;
        std::cout << getEnvironmentVariable("JAVA_HOME").value_or("(none)") << std::endl;
    }

    /*
    extern char **environ;

    void test() {
        int count = 0;
        while (environ[count++] != nullptr)
            std::cout <<  environ[count] << std::endl;
    }
     */
};

void Environment::TestAll() {
    EnvironmentVariable();
    // setVariable();
};


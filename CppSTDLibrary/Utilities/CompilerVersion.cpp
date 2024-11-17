/**============================================================================
Name        : CompilerVersion.cpp
Created on  : 17.11.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CompilerVersion.cpp
============================================================================**/

#include "CompillerVersion.h"

#include <iostream>

void CompilerVersion::TestAll()
{
    std::cout << __cplusplus << std::endl;

    // Check compiler version
#if defined(__GNUC__)
    std::cout << "Compiler: GCC" << std::endl;
    std::cout << "Version: " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << std::endl;
#elif defined(_MSC_VER)
    std::cout << "Compiler: MSVC" << std::endl;
    std::cout << "Version: " << _MSC_VER << std::endl;
#elif defined(__clang__)
    std::cout << "Compiler: Clang" << std::endl;
    std::cout << "Version: " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__ << std::endl;
#else
    std::cout << "Compiler: Unknown" << std::endl;
#endif


    std::cout << std::endl;

    // Using __cplusplus macro to check C++ version
#ifdef __cplusplus

    if (__cplusplus == 202400L)
        std::cout << "C++26" << std::endl;
    else if (__cplusplus == 202302L)
        std::cout << "C++23" << std::endl;
    else if (__cplusplus == 202002L)
        std::cout << "C++20" << std::endl;
    else if (__cplusplus == 201703L)
        std::cout << "C++17" << std::endl;
    else if (__cplusplus == 201402L)
        std::cout << "C++14" << std::endl;
    else if (__cplusplus == 201103L)
        std::cout << "C++11" << std::endl;
    else if (__cplusplus == 199711L)
        std::cout << "C++98" << std::endl;
    else
        std::cout << "Unknown C++ version" << std::endl;
#else
    std::cout << "Not a C++ compiler" << std::endl;
#endif


}
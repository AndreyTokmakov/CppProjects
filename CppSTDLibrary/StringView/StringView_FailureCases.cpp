/**============================================================================
Name        : StringView_FailureCases.cpp
Created on  : 01.04.2026
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "StringViewTests.h"

#include <iostream>
#include <string>
#include <string_view>

namespace
{
    std::string_view bad()
    {
        std::string const local = "hello world, i will disappear";
        return local;  // string_view returned, local destroyed → dangling pointer!
    }

    void dangling_references()
    {
        const std::string_view sv = bad();  // sv.data() points to freed memory
        std::cout << sv;  // undefined behavior — crash, garbage, or "works" by accident    
    }
}

namespace
{
    void temporary_strings()
    {
        // The temporary std::string is destroyed at end of this statement! sv is already dangling.
        const std::string_view sv = std::string("hello");

        // // Same problem. Concatenation creates a temporary, it's immediately destroyed.
        const std::string_view sv2 = std::string("prefix") + std::string("suffix");
    }
}

//INFO: https://towardsdev.com/cpp17-string-view-stop-copying-strings-2bf748dc3a12
void StringView::failure_cases::TestAll()
{
    dangling_references();
}

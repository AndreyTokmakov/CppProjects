/**============================================================================
Name        : InlineNamespaces.h
Created on  : 20.06.2022.
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : InlineNamespaces
============================================================================**/

#include "InlineNamespaces.h"

#include <iostream>

namespace InlineNamespaces
{
    namespace Program
    {
        namespace Version1
        {
            void printVersion() {
                std::cout << "Version1" << std::endl;
            }

            bool isFirstVersion() {
                return true;
            }
        }

        inline
        namespace Version2
        {
            void printVersion() {
                std::cout << "Version2" << std::endl;
            }

            bool isSecondVersion() {
                return true;
            }
        }
    }


    void CheckVersion()
    {
        Program::printVersion(); // Will call Program::Version2::printVersion()
    }
}

/**
 * All members of an inline namespace are treated as if they were part of its parent namespace,
 * allowing specialization of functions and easing the process of versioning.
 * This is a transitive property, if A contains B, which in turn contains C and both B and C are inline namespaces,
 * C's members can be used as if they were on A.
 */

void InlineNamespaces::TestAll()
{
    InlineNamespaces::CheckVersion();
}
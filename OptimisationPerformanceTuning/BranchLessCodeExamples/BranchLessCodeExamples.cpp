/**============================================================================
Name        : BranchLessCodeExamples.cpp
Created on  : 15.08.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : BranchLessCodeExamples.cpp
============================================================================**/

#include "BranchLessCodeExamples.h"

namespace BranchLessCodeExamples::ExampleOne
{

    void Old()
    {
#if 0
        unsigned long* p1 = ...; // Data
        bool* b1 = ...; // Unpredictable condition
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            if (b1[i]) {
                a1 += p1[i];
            } else {
                a2 += p1[i];
            }
        }
#endif
    }
}

void BranchLessCodeExamples::TestAll()
{

}
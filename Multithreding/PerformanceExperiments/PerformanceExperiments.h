/**============================================================================
Name        : PerformanceExperiments.h
Created on  : 26.02.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Multithreading performance experiments
============================================================================**/

#ifndef CPPPROJECTS_PERFORMANCEEXPERIMENTS_H
#define CPPPROJECTS_PERFORMANCEEXPERIMENTS_H

namespace PerformanceExperiments
{
    void TestAll();

    namespace Multithreading_Performance { void TestAll(); }
    namespace McMp_LockFree_Queue_vs_CV_Queue { void TestAll(); }
    namespace McMp_LockFree_Queue_vs_CV_Queue_2 { void TestAll(); }
    namespace Mutex_vs_AtomicLock { void benchmarks(); }
    namespace ConditionVariable_vs_Atomic { void benchmarks(); }
    namespace SpinLock { void benchmarks(); }
};

#endif //CPPPROJECTS_PERFORMANCEEXPERIMENTS_H

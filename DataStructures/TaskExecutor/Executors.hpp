/**============================================================================
Name        : TaskExecutor.hpp
Created on  : 06.05.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TaskExecutor.hpp
============================================================================**/

#ifndef CPPPROJECTS_TASKEXECUTOR_HPP
#define CPPPROJECTS_TASKEXECUTOR_HPP
#define CPPPROJECTS_TASKEXECUTOR_HPP

namespace task_executor
{
    void TestAll();
    namespace TaskExecutor1 { void TestAll(); }
    namespace TaskExecutor2 { void TestAll(); }
}

#endif //CPPPROJECTS_TASKEXECUTOR_HPP

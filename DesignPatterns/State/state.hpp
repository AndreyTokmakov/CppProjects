/**============================================================================
Name        : IState.hpp
Created on  : 27.11.2022
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : IState
============================================================================**/

#ifndef CPPPROJECTS_STATE_H
#define CPPPROJECTS_STATE_H

namespace state
{
    void TestAll();

    namespace climate_control { void TestAll(); }
    namespace tcp_state_machine { void TestAll(); }
    namespace tcp_state_machine_visitor { void TestAll(); }
}

#endif // !CPPPROJECTS_STATE_H
/**============================================================================
Name        : IState.cpp
Created on  : 27.11.2022
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : IState
============================================================================**/

#include "state.hpp"

void state::TestAll()
{
    // climate_control::TestAll();
    // tcp_state_machine::TestAll();
    // tcp_state_machine_visitor::TestAll();
    static_transition_table::TestAll();
}

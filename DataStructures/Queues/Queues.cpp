/**============================================================================
Name        : Queues.cpp
Created on  : 10.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Queues.cpp
============================================================================**/

#include "Queues.hpp"

void queues::TestAll()
{
    // priority_dispatcher::TestAll();
    multi_level_queue::TestAll();
}

/*
Priority Queue
      ↓
Priority Dispatcher
      ↓
Multi-Level Queue
      ↓
Weighted Fair Queue
      ↓
Aging
      ↓
Priority Inversion
      ↓
Priority Inheritance
      ↓
Deadline Scheduling
      ↓
EDF
*/
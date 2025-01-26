/**============================================================================
Name        : XTR.cpp
Created on  : 26.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : XTR.cpp
============================================================================**/

#include "XTR.h"
#include <xtr/logger.hpp>



void XTR::TestAll()
{
    xtr::logger log;
    auto s = log.get_sink("Test");
    XTR_LOG(s, "Hello world");
}
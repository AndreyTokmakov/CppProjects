/**============================================================================
Name        : Coroutines.cpp
Created on  : 08.07.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Coroutines
============================================================================**/

#include "ModulesKernel.h"

#include <iostream>
#include <string_view>

#include <linux/kernel.h>
#include <linux/module.h>



namespace ModulesKernel
{
    int init_module()
    {
        // pr_info("Hello world\n");
        return 0;
    }

    void cleanup_module()
    {
        // pr_info("Goodbye world\n");
    }


}

void ModulesKernel::TestAll()
{
    std::cout << "ModulesKernel\n";

}


/**
ls -l /lib/modules/$(uname -r)/build
**/
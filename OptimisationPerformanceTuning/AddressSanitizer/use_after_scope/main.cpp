/**============================================================================
Name        : main.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>

int main(int argc, char **argv)
{
    int *p = 0;
    {
        int x = 0;
        p = &x;
    }
    *p = 5;
    return 0;
}

/** [32, 36) 'x' (line 17) <== Memory access at offset 32 is inside this variable **/

/*
==23246==ERROR: AddressSanitizer: stack-use-after-scope on address 0x7f111b300020 at pc 0x000000401278 bp 0x7ffd8e88e0b0 sp 0x7ffd8e88e0a8
WRITE of size 4 at 0x7f111b300020 thread T0
    #0 0x401277 in main (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/AddressSanitizer/use_after_scope/test_bin+0x401277)
    #1 0x7f111d77d082 in __libc_start_main ../csu/libc-start.c:308
    #2 0x4010bd in _start (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/AddressSanitizer/use_after_scope/test_bin+0x4010bd)

Address 0x7f111b300020 is located in stack of thread T0 at offset 32 in frame
    #0 0x401185 in main (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/AddressSanitizer/use_after_scope/test_bin+0x401185)

  This frame has 1 object(s):
    [32, 36) 'x' (line 17) <== Memory access at offset 32 is inside this variable
HINT: this may be a false positive if your program uses some custom stack unwind mechanism, swapcontext or vfork
      (longjmp and C++ exceptions *are* supported)

*/
/**============================================================================
Name        : main.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>

int *ptr {nullptr};

__attribute__((noinline))
void FunctionThatEscapesLocalObject() {
    int local[100];
    ptr = &local[0];
}

int main(int argc, char **argv)
{
    FunctionThatEscapesLocalObject();
    return ptr[argc];                   /** <---- Accessing deleted memory .. as part of 'local' variable **/
}

/*

==13896==ERROR: AddressSanitizer: stack-use-after-return on address 0x7fbe10400034 at pc 0x0000004012c6 bp 0x7ffd44714040 sp 0x7ffd44714038
READ of size 4 at 0x7fbe10400034 thread T0
    #0 0x4012c5 in main (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/AddressSanitizer/use_after_return/test_bin+0x4012c5)
    #1 0x7fbe125a0082 in __libc_start_main ../csu/libc-start.c:308
    #2 0x4010bd in _start (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/AddressSanitizer/use_after_return/test_bin+0x4010bd)

Address 0x7fbe10400034 is located in stack of thread T0 at offset 52 in frame
    #0 0x401185 in FunctionThatEscapesLocalObject() (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/AddressSanitizer/use_after_return/test_bin+0x401185)

  This frame has 1 object(s):
    [48, 448) 'local' (line 16) <== Memory access at offset 52 is inside this variable
HINT: this may be a false positive if your program uses some custom stack unwind mechanism, swapcontext or vfork
      (longjmp and C++ exceptions *are* supported)
SUMMARY: AddressSanitizer: stack-use-after-return (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/AddressSanitizer/use_after_return/test_bin+0x4012c5) in main
Shadow bytes around the buggy address:

*/
/**============================================================================
Name        : main.cpp
Created on  : 23.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>

int x[100];

int main()
{
    printf("Hello!\n");
    x[100] = 5; // Boom!
    return EXIT_SUCCESS;
}


/**
=================================================================
==36585==ERROR: AddressSanitizer: global-buffer-overflow on address 0x0000004043b0 at
 pc 0x0000004011b4 bp 0x7fffe9e59bd0 sp 0x7fffe9e59bc8
WRITE of size 4 at 0x0000004043b0 thread T0
    #0 0x4011b3 in main (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/AddressSanitizer/buffer_overflow/test_bin+0x4011b3)
    #1 0x7fdae3cf6082 in __libc_start_main ../csu/libc-start.c:308
    #2 0x4010bd in _start (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/AddressSanitizer/buffer_overflow/test_bin+0x4010bd)

0x0000004043b0 is located 0 bytes after global variable 'x' defined in 'main.cpp:12:5' (0x404220) of size 400
SUMMARY: AddressSanitizer: global-buffer-overflow (/home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/AddressSanitizer/buffer_overflow/test_bin+0x4011b3) in main
Shadow bytes around the buggy address:
*/
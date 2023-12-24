/**============================================================================
Name        : main.cpp
Created on  : 23.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>

int main(int argc, char **argv)
{
    int *array = new int[100];
    delete [] array;
    return array[argc];  // BOOM
}

/**
==19338==ERROR: AddressSanitizer: heap-use-after-free on address 0x614000000044 at pc 0x000000401202 bp 0x7fffeb5553f0 sp 0x7fffeb5553e8
READ of size 4 at 0x614000000044 thread T0
    #0 0x401201 in main (.../use_after_free/test_bin+0x401201)
    #1 0x7f08c534e082 in __libc_start_main ../csu/libc-start.c:308
    #2 0x4010cd in _start (.../use_after_free/test_bin+0x4010cd)

0x614000000044 is located 4 bytes inside of 400-byte region [0x614000000040,0x6140000001d0)
freed by thread T0 here:
    #0 0x7f08c59f9098 in operator delete[](void*) (/home/andtokm/DiskS/Utils/bin/gcc-13.1/lib64/libasan.so.8+0xdc098)
    #1 0x4011b5 in main (.../use_after_free/test_bin+0x4011b5)
    #2 0x7f08c534e082 in __libc_start_main ../csu/libc-start.c:308

previously allocated by thread T0 here:
    #0 0x7f08c59f8688 in operator new[](unsigned long) (/home/andtokm/DiskS/Utils/bin/gcc-13.1/lib64/libasan.so.8+0xdb688)
    #1 0x40119e in main (.../use_after_free/test_bin+0x40119e)
    #2 0x7f08c534e082 in __libc_start_main ../csu/libc-start.c:308
**/
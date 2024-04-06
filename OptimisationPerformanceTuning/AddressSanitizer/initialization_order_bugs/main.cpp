/**============================================================================
Name        : main.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>

#include <stdio.h>

extern int extern_global;

int __attribute__((noinline)) read_extern_global()
{
    return extern_global;
}

int main(int argc, char **argv)
{
    int x = read_extern_global() + 1;
    printf("%d\n", x);
    return 0;
}
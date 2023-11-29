/**============================================================================
Name        : CodeInspection.h
Created on  : 29.11.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CodeInspection.h
============================================================================**/

int value { 0 };

void add(int v)
{
    value += v;
}

int foo()
{
    add(1);
    add(2);
    return value;
}
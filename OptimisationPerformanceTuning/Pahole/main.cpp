/**============================================================================
Name        : main.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

struct StructOne
{
    int a {0};
    double b {0.0};
};

struct StructOnePacked1
{
    int a {0};
    double b {0.0};
} __attribute__((packed));

struct StructOnePacked2
{
    int a {0};
    double b {0.0};
} __attribute__((packed, aligned(1)));


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    StructOne a;
    StructOnePacked1 b;
    StructOnePacked2 c;


    return 0;
}

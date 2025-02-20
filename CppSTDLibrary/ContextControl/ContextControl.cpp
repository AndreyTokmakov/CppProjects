/**============================================================================
Name        : ContextControl.cpp
Created on  : 21.06.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ContextControl.h
============================================================================**/

#include "ContextControl.h"

#include <iostream>
#include <ucontext.h>

static ucontext_t caller_context;
static ucontext_t coroutine_context;

namespace ContextControl
{
    void print_hello_and_suspend()
    {
        std::cout << "Hello";
        // point of control transfer to the calling party, switch to the caller_context context
        // the current execution point is saved in the coroutine_context context,
        // after control is returned, execution will continue from this point.
        swapcontext(&coroutine_context, &caller_context);
    }

    void simple_coroutine()
    {
        // point of first control transfer in coroutine_context to demonstrate the benefits
        // using stack let's make a nested call to print_hello_and_suspend.
        print_hello_and_suspend();
        // the print_hello_and_suspend function suspended the execution of the coroutine
        // after control returns, we will print Coroutine! and finish the work, control will be transferred to the context,
        // the pointer to which is stored in coroutine_context.uc_link, i.e. caller_context
        std::cout << "Coroutine!" << std::endl;
    }
}


/// INFO: https://habr.com/ru/articles/519464/
void ContextControl::TestAll()
{
    // Coroutine stack.
    char stack[256];

    // Initialize the coroutine context coroutine_context uc_link points to caller_context, the return
    // point when the coroutine terminates. uc_stack stores the stack pointer and size
    coroutine_context.uc_link          = &caller_context;
    coroutine_context.uc_stack.ss_sp   = stack;
    coroutine_context.uc_stack.ss_size = sizeof(stack);
    getcontext(&coroutine_context);

    // Filling coroutine_context: The context is set up so that when switching to it
    // execution starts at the entry point of the simple_coroutine function
    makecontext(&coroutine_context, simple_coroutine, 0);

    // transfer control to the coroutine, switch to the coroutine_context context in the caller_context context
    // save the current execution point, after returning control, execution will continue from this point.
    swapcontext(&caller_context, &coroutine_context);

    // the coroutine paused its execution and returned control, outputting a space
    std::cout << " ";

    // pass control back to the coroutine.
    swapcontext(&caller_context, &coroutine_context);
}

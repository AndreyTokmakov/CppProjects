/**============================================================================
Name        : Execution.cpp
Created on  : 05.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Execution
============================================================================**/

#include "Execution.h"

#ifdef EXECUTION_ENABLED

#include <stdexec/execution.hpp>
#include <exec/static_thread_pool.hpp>
#include <iostream>


// https://godbolt.org/
void test()
{
    // Declare a pool of 2 worker threads to handle file loading:
    exec::static_thread_pool network_pool (2);
    // Get a handle to the thread pool:
    auto network_sched = network_pool.get_scheduler();

    // Declare a pool of 2 worker threads to handle parsing content operations:
    exec::static_thread_pool parsing_pool (2);
    // Get a handle to the thread pool:
    auto parsing_sched = parsing_pool.get_scheduler();

    // Heavy task that loads some content from the net:
    auto readFromNet = []() -> std::string {
        return "this content obtained from the net";
    };

    // Another heavy task that parses obtained content
    auto parsingContent = [](const std::string &content) {
        return "Parsed:" + content;
    };

    // Create a work that would be later executed
    // And specify with which scheduler it should be invoked
    auto work = stdexec::schedule(network_sched) |
          stdexec::then(readFromNet) |
          // use another scheduler for parsing operations
          stdexec::continues_on(parsing_sched) |
          stdexec::then(parsingContent);

    // Launch the work and wait for the result
    auto [parsed] = stdexec::sync_wait(std::move(work)).value();

    // Print the results:
    std::cout << "Resulting content: " << parsed << std::endl;
    return 0;
}

#endif

void Execution::TestAll()
{

}
